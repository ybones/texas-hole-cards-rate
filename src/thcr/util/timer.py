# -*- coding: utf-8 -*-


def print_run_time(func):
    import time

    def wrapper(*args, **kwargs):
        start_time = time.time()
        func_return = func(*args, **kwargs)
        end_time = time.time()
        print ('[%s] run time is %.2f' % (func.__name__, end_time - start_time))
        return func_return

    return wrapper
