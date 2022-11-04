"""Custom exceptions for the SIT base class"""


class SITError(Exception):
    """Base exception class"""

    def __init__(self):
        pass


class APIException(SITError):
    """Exceptions for unsupported attributes"""

    def __init__(self, message):
        pass


class IPCException(SITError):
    """Exceptions for invalid or unsupported interprocess communication"""

    def __init__(self, message):
        pass


class PortException(SITError):
    """Exceptions for invalid port types and empty ports"""

    def __init__(self, message):
        pass


class SignalFormatException(SITError):
    """Exceptions for invalid signal format"""

    def __init__(self, message):
        pass


class TemplateFileNotFound(SITError):
    """Exceptions for missing or invalid path to template file"""

    def __init__(self, message):
        pass
