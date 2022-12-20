"""Custom exceptions for the SIT base class"""


class SITError(Exception):
    """Base exception class"""

    pass


class IPCException(SITError):
    """Exceptions for invalid or unsupported interprocess communication"""

    pass


class PortException(SITError):
    """Exceptions for invalid port types and empty ports"""

    pass


class SignalFormatException(SITError):
    """Exceptions for invalid signal format"""

    pass


class TemplateFileNotFound(SITError):
    """Exceptions for missing or invalid path to template file"""

    pass
