#!/usr/bin/env python
# -*- coding: utf-8 -*-

from pprint import pprint
import sys

if __name__ == "__main__":

    with open(sys.argv[-1]) as mem_dump_file:

        cure_progress = 0

        stats = [{
                "stat": "severity",
                "val": None
            }, {
                "stat": "infectivity",
                "val": None
            }, {
                "stat": "fatality",
                "val": None
            }, {
                "stat": "birth_rate",
                "val": None
            }, {
                "stat": "cure_threshold",
                "val": None
        }]
        stats_dump = False
        stats_line = 0

        hist = [{"cure": x, "first_date": 0, "inf": 0, "dead": 0} for x in range(100)]

        total_inf = 0
        total_dead = 0

        for line in mem_dump_file:
            addr, data = line.split()
            pop_inf, pop_dead, cure = int(data[0:3]), int(data[3:6]), int(data[6:8])
            if cure_progress != cure:
                cure_progress = cure
                if not hist[cure_progress]["first_date"] and cure:
                    hist[cure_progress]["first_date"] = int(addr)

            if not stats_dump and cure == 99:
                stats_dump = True
            elif stats_dump and cure != 99:
                if stats_line < 4:
                    stats[stats_line]["val"] = float("0."+ data[1:])
                elif stats_line == 4:
                    stats[stats_line]["val"] = int(data)
                stats_line += 1
                # print(int(addr), stats)
            else:
                total_inf += pop_inf
                total_dead += pop_dead
                print(int(addr), pop_inf, pop_dead, cure)
                hist[cure_progress]["inf"] += pop_inf
                hist[cure_progress]["dead"] += pop_dead

        print(f"Cure started on day: {hist[1]['first_date']}")
        print(f"Total infected: {total_inf}")
        print(f"Total dead: {total_dead}")
        print("Statistics:")
        pprint(stats)

        pprint(hist)
