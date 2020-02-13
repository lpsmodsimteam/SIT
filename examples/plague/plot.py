#!/usr/bin/env python
# -*- coding: utf-8 -*-

import json
import sys

import plotly.graph_objs as go

if __name__ == "__main__":

    data = {}
    with open(sys.argv[-1]) as stats_dump_file:
        data = json.load(stats_dump_file)

        fig = go.Figure()
        fig.add_trace(go.Scatter(
            x=data["plot_data"]["date"], y=data["plot_data"]["inf_total"],
            name="population infected", fill="tozeroy",
            line={"color": "rgb(255,154,0)"}, mode="lines+markers"
        ))
        fig.add_trace(go.Scatter(
            x=data["plot_data"]["date"], y=data["plot_data"]["dead_total"],
            name="population dead", fill="tozeroy",
            line={"color": "rgb(255,0,0)"}, mode="lines+markers"
        ))
        fig.update_layout(barmode="stack")
        fig.show()
