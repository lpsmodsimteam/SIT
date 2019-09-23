# SST Interoperable: PyRTL

## Black Box Code Generation

The following snippet demonstrates an example usage of the class to generate black box components
for `demo`.

```python
# demo/generate_bbox.py

import boilerplate

DEMO_ARGS = {
    "module" : "demo",
    "lib" : "demo_lib",
    "ipc" : "sock",
    "desc" : "Demo SST Black Box Component",
}

demo_obj = boilerplate.PyRTL(**DEMO_ARGS)
demo_obj.set_ports((
    ("3", "clock", "clock"),
    ("2", "input1", "input"),
    ("4", "input2", "input"),
    ("4", "data_out", "output"),
)).generate_bbox()
```

Running the script `python3 generate_bbox.py` will generate the boilerplate code necessary to
establish interoperability between the black box components of `demo`.

## Installation

PyRTL can be installed via pip:
```bash
pip install pyrtl
```

## Requirements

The following table summarizes the minimum versions of the languages and libraries supported:

|Requirement|Version|
|-----------|-------|
|PyRTL      |0.8.7  |
