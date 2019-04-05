#!/usr/bin/env python3

import os
import subprocess
import itertools
from collections import defaultdict
import re
from typing import Iterator, List, Dict, Set


EVENT_HEADER = '../../measurement/include/SCOREP_Events.h'
EVENT_PATTERN = 'SCOREP_Io[A-Za-z]*'


class EventWrapper:
    def __init__(self, file_path, func_get_func_name, func_is_wrapper_func):
        self.m_get_func_name = func_get_func_name
        self.m_is_wrapper_func = func_is_wrapper_func
        self.m_file_path = file_path

    def get_file_basename(self) -> str:
        return os.path.basename(self.m_file_path)


def libwrap_get_func_name(line: str) -> str:
    ret = re.search('SCOREP_LIBWRAP_FUNC_NAME.*', line)
    assert(ret is not None)
    assert(ret.group() is not None)
    return ret.group().replace('(', ',').split(" ")[1]


def libwrap_is_wrapper_func(line: str) -> bool:
    return "=SCOREP_LIBWRAP_FUNC_NAME" in line


def mpi_is_wrapper_func(line: str) -> bool:
    return "=MPI_File" in line


def mpi_get_func_name(line: str) -> str:
    ret = re.search('MPI(_[A-Za-z]*)*', line)
    assert(ret is not None)
    assert(ret.group() is not None)
    return ret.group()


def get_io_events(event_file: str, event_pattern: str):
    """
    Searches for event pattern in a given event file
    and returns a list of matching events.
    If no event was found, it returns None.
    """
    cp = subprocess.run(["grep", "-o", event_pattern, event_file], capture_output=True )
    if cp.returncode == 0:
        return cp.stdout.decode("utf-8").split("\n")
    print("Grep falied with exit code {}".format(cp.returncode))
    return None


def git_grep(file: str, event_pattern: str):
    """
    Searches for a particular event in a implemented wrapper.
    It uses git grep to return the function context that contains the event
    specified by the event_pattern.
    """
    cp = subprocess.run(["git", "grep", "-p", event_pattern, file], capture_output=True)
    if cp.returncode == 0:
        return cp.stdout.decode("utf-8").split("\n")
    print("Git grep falied with exit code {}".format(cp.returncode))
    return None


def lookahead(iterobj: Iterator[list]) -> (Iterator[list], Iterator[list]):
    """
    Returns next and current iterator.
    """
    prev, cur = itertools.tee(iterobj)
    return next(cur), prev


# TODO Just yield the events and delete the try and catch.
def read_events(iterobj: Iterator[list],
                events: List[str],
                file: str,
                defined_events: List[str],
                event_pattern: str) -> Iterator[list]:
    """
    Reads all events of a function context and stores it in events.
    """
    while True:
        try:
            iter_ahead, iterobj = lookahead(iterobj)
            if "{}=".format(file) in iter_ahead:
                return iterobj
            l = next(iterobj)
            m = re.search(event_pattern, l)
            if m is not None and m.group() in defined_events:
                events.add(m.group())
        except StopIteration:
            return iterobj


def get_wrapper_events(wrapper: EventWrapper, defined_events: List[str], event_pattern: str) -> Dict[str, Set[str]]:
    """
    Determines triggered events (corresponding to the event pattern) of wrapper functions.
    It returns a dictionary of the following structure:
    Dict{
        function_name: Set(occurring_events)
    }
    """
    out_lines = git_grep(wrapper.m_file_path, event_pattern)
    if out_lines is not None:
        wrapper_events = {}
        iter_obj = iter(out_lines)
        l = next(iter_obj)
        while True:
            try:
                if wrapper.m_is_wrapper_func(l):
                    func_name = wrapper.m_get_func_name(l)
                    next(iter_obj)
                    wrapper_events[func_name] = set()
                    iter_obj = read_events(iter_obj, wrapper_events[func_name], wrapper.get_file_basename(), defined_events, event_pattern)
                l = next(iter_obj)
            except StopIteration:
                return wrapper_events


def classify_wrapper(wrapper_events: Dict[str, Set[str]],
                     classified_wrapper: Dict[str, Dict[str, Set[str]]]) -> None:
    """
    Classify wrapper functions regarding their occurring events and
    fills a dictionary with the following structure:
    Dict{
        class_name: Dict{
            function_name: Set(occurring_events)
        }
    }
    """

    def is_nonblocking_transfer(events):
        if 'SCOREP_IoOperationBegin' in events and len(events) == 1:
            return True
        elif 'SCOREP_IoOperationBegin' in events and 'SCOREP_IoOperationIssued' in events:
            return True
        else:
            return False

    def is_blocking_transfer(events):
        return 'SCOREP_IoOperationBegin' in events and 'SCOREP_IoOperationComplete' in events

    for func, events in wrapper_events.items():
        if len(events) == 0:
            continue
        if 'SCOREP_IoCreateHandle' in events:
            classified_wrapper["SCOREP_SCORE_EVENT_IO_CREATE"][func] = events
        elif 'SCOREP_IoDuplicateHandle' in events:
            classified_wrapper["SCOREP_SCORE_EVENT_IO_DUPLICATE"][func] = events
        elif 'SCOREP_IoSeek' in events:
            classified_wrapper["SCOREP_SCORE_EVENT_IO_SEEK"][func] = events
        elif 'SCOREP_IoDeleteFile' in events:
            classified_wrapper["SCOREP_SCORE_EVENT_IO_DELETE"][func] = events
        elif 'SCOREP_IoAcquireLock' in events:
            classified_wrapper["SCOREP_SCORE_EVENT_IO_ACQUIRE_LOCK"][func] = events
        elif 'SCOREP_IoOperationCancelled' in events:
            classified_wrapper["SCOREP_SCORE_EVENT_IO_OPERATION_CANCELLED"][func] = events
        elif 'SCOREP_IoReleaseLock' in events:
            classified_wrapper["SCOREP_SCORE_EVENT_IO_RELEASE_LOCK"][func] = events
        elif 'SCOREP_IoDestroyHandle' in events:
            classified_wrapper["SCOREP_SCORE_EVENT_IO_CLOSE"][func] = events
        elif is_blocking_transfer(events):
            classified_wrapper["SCOREP_SCORE_EVENT_IO_BLOCKING_TRANSFER"][func] = events
        elif is_nonblocking_transfer(events):
            classified_wrapper["SCOREP_SCORE_EVENT_IO_NONBLOCKING_TRANSFER_BEGIN"][func] = events
        elif 'SCOREP_IoOperationComplete' in events or 'SCOREP_IoOperationCancelled' in events:
            classified_wrapper["SCOREP_SCORE_EVENT_IO_NONBLOCKING_TRANSFER_END"][func] = events
        else:
            print("{} not classified : {}".format(func, events))


def gen_scorep_score_event_list(classify_wrapper_funcs: Dict[str, Dict[str, Set[str]]], file=None) -> None:
    """
    Creates and prints score event lists for all classses with their corresponding functions.
    """
    for score_class in classified_wrapper_funcs:
        print("#define {} \\".format(score_class), file=file)
        for i,func in enumerate(classified_wrapper_funcs[score_class]):
            if i == len(classified_wrapper_funcs[score_class]) - 1:
                print("\tSCOREP_SCORE_EVENT( \"{}\" )".format(func), file=file)
                print("", file=file)
            else:
                print("\tSCOREP_SCORE_EVENT( \"{}\" )\\".format(func), file=file)


def get_most_events(wrapper_events: Dict[str, Set[str]]) -> Set[str]:
    """
    Returns the event set with the most items.
    """
    k = max(wrapper_events, key=lambda s: len(wrapper_events[s]))
    return wrapper_events[k]


def rm_prefix(event):
    """
    Deletes the SCOREP_ prefix of string.
    """
    return event.replace("SCOREP_", "")


def gen_register_event(classify_wrapper_funcs: Dict[str, Dict[str, Set[str]]], file=None) -> None:
    """
    Creates and prints score event registrations for all classses with their corresponding events.
    """
    for score_class in classified_wrapper_funcs:
        print("region_set.clear();", file=file)
        print("{};".format(score_class), file=file)
        events = get_most_events(classified_wrapper_funcs[score_class])
        for event in events:
            print("registerEvent( new SCOREP_Score_NameMatchEvent( \"{}\",\n"\
                  "                                                 region_set,\n"\
                  "                                                 true ) );".format(rm_prefix(event)), file=file)


posix_wrapper = EventWrapper("../../adapters/io/posix/scorep_posix_io_wrap.c",
                             libwrap_get_func_name,
                             libwrap_is_wrapper_func)
isoc_wrapper = EventWrapper("../../adapters/io/posix/scorep_posix_io_wrap_isoc.c",
                             libwrap_get_func_name,
                             libwrap_is_wrapper_func)
mpi_wrapper = EventWrapper("../../adapters/mpi/SCOREP_Mpi_Io.c",
                           mpi_get_func_name,
                           mpi_is_wrapper_func)

defined_io_events = get_io_events(EVENT_HEADER, EVENT_PATTERN)
classified_wrapper_funcs = defaultdict(dict)
for wrapper in [mpi_wrapper, posix_wrapper, isoc_wrapper]:
    wrapper_events = get_wrapper_events(wrapper, defined_io_events, EVENT_PATTERN)
    classify_wrapper(wrapper_events, classified_wrapper_funcs)

gen_scorep_score_event_list(classified_wrapper_funcs)
gen_register_event(classified_wrapper_funcs)
