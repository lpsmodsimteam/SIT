#!/usr/bin/env python
# -*- coding: utf-8 -*-

import datetime
from pprint import pprint
import sys

import chart_studio.plotly as py
import plotly.graph_objs as go

if __name__ == "__main__":

    with open(sys.argv[-1]) as mem_dump_file:

        cure_progress = 0
        today = datetime.date.today()

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

        hist = {
            "cure": [x for x in range(100)],
            "first_date": [0 for _ in range(100)],
            "inf": [0 for _ in range(100)],
            "dead": [0 for _ in range(100)]
        }

        full_data = {
            "date": [],
            "inf": [],
            "dead": [],
            "cure": []
        }

        total_inf = 0
        total_dead = 0

        month = today.month
        _monthly_inf = 0
        _monthly_dead = 0

        for line in mem_dump_file:
            addr, data = line.split()
            pop_inf, pop_dead, cure = int(data[0:3]), int(data[3:6]), int(data[6:8])
            if cure_progress != cure:
                cure_progress = cure
                if not hist["first_date"][cure_progress] and cure:
                    hist["first_date"][cure_progress] = int(addr)

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
                # print(int(addr), pop_inf, pop_dead, cure)
                # hist["inf"][cure_progress] += pop_inf
                # hist["dead"][cure_progress] += pop_dead

                _date = today + datetime.timedelta(int(addr))
                _month = _date.month
                _monthly_inf += pop_inf
                _monthly_dead += pop_dead
                if month != _month:
                    month = _month
                    full_data["date"].append(_date)
                    full_data["inf"].append(_monthly_inf)
                    full_data["dead"].append(_monthly_dead)
                    full_data["cure"].append(cure)
                    _monthly_inf = 0
                    _monthly_dead = 0

        print(f"Cure started on day: {today + datetime.timedelta(hist['first_date'][1])}")
        print(f"Total infected: {total_inf}")
        print(f"Total dead: {total_dead}")
        print("Statistics:")
        pprint(stats)

        # pprint(full_data)

        fig = go.Figure()
        fig.add_trace(go.Bar(
            x=full_data["date"], y=full_data["inf"], name="infected"
        ))
        fig.add_trace(go.Bar(
            x=full_data["date"], y=full_data["dead"], name="dead"
        ))
        fig.update_layout(barmode='stack')
        fig.show()
