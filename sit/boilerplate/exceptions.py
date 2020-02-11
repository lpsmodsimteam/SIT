#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""Custom exceptions for the BoilerPlate base class"""


class BoilerPlateError(Exception):
    """Base exception class"""

    def __init__(self):
        pass


class APIException(BoilerPlateError):
    """Exceptions for unsupported attributes"""

    def __init__(self, message):
        pass


class IPCException(BoilerPlateError):
    """Exceptions for invalid or unsupported interprocess communication"""

    def __init__(self, message):
        pass


class PortException(BoilerPlateError):
    """Exceptions for invalid port types and empty ports"""

    def __init__(self, message):
        pass


class SignalFormatException(BoilerPlateError):
    """Exceptions for invalid signal format"""

    def __init__(self, message):
        pass


class TemplateFileNotFound(BoilerPlateError):
    """Exceptions for missing or invalid path to template file"""

    def __init__(self, message):
        pass
