#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""Custom exceptions for the BoilerPlate base class"""


class BoilerPlateError(Exception):
    def __init__(self):
        pass


class PortException(BoilerPlateError):

    def __init__(self, message):
        pass


class IPCException(BoilerPlateError):

    def __init__(self, message):
        pass


class SignalFormatException(BoilerPlateError):

    def __init__(self, message):
        pass


class TemplateFileNotFound(BoilerPlateError):

    def __init__(self, message):
        pass
