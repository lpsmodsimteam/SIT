#!/usr/bin/env python
# -*- coding: utf-8 -*-

import datetime
import json
import sys

if __name__ == "__main__":

    stats = {
        "severity": None,
        "infectivity": None,
        "fatality": None,
        "birth_rate": None,
        "cure_threshold": None,
        "cure_started_day": None,
        "total_inf": None,
        "total_dead": None,
        "days": None,
        "plot_data": {
            "date": [],
            "inf_month": [],
            "dead_month": [],
            "inf_total": [],
            "dead_total": [],
            "cure": []
        }
    }

    with open(sys.argv[-1]) as mem_dump_file:

        stats_dump = False
        stats_line = 0

        total_inf = 0
        total_dead = 0

        today = datetime.date.today()
        month = today.month
        _monthly_inf = 0
        _monthly_dead = 0

        for line in mem_dump_file:
            addr, data = line.split()
            pop_inf, pop_dead, cure = int(data[0:3]), int(data[3:6]), int(data[6:8])
            if not stats["cure_started_day"] and cure:
                stats["cure_started_day"] = int(addr)

            if not stats_dump and cure == 99:
                stats_dump = True
            elif stats_dump and cure != 99:
                if stats_line == 0:
                    stats["severity"] = float("0." + data[1:])
                elif stats_line == 1:
                    stats["infectivity"] = float("0." + data[1:])
                elif stats_line == 2:
                    stats["fatality"] = float("0." + data[1:])
                elif stats_line == 3:
                    stats["birth_rate"] = float("0." + data[1:])
                elif stats_line == 4:
                    stats["cure_threshold"] = int(data)
                stats_line += 1

            else:
                total_inf += pop_inf
                total_dead += pop_dead

                _date = today + datetime.timedelta(int(addr))
                _month = _date.month
                _monthly_inf += pop_inf
                _monthly_dead += pop_dead
                if month != _month:
                    month = _month
                    stats["plot_data"]["date"].append(_date.isoformat())
                    stats["plot_data"]["inf_total"].append(total_inf)
                    stats["plot_data"]["dead_total"].append(total_dead)
                    stats["plot_data"]["inf_month"].append(_monthly_inf)
                    stats["plot_data"]["dead_month"].append(_monthly_dead)
                    stats["plot_data"]["cure"].append(cure)
                    _monthly_inf = 0
                    _monthly_dead = 0

        stats["total_inf"] = total_inf
        stats["total_dead"] = total_dead
        stats["days"] = int(addr) - 5

    print(f"Cure started on {today + datetime.timedelta(stats['cure_started_day'])}")
    with open("data.json", "w") as stats_dump_file:
        json.dump(stats, stats_dump_file)
