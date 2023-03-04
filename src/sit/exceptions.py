"""Custom exceptions for the SIT base class"""


class SITError(Exception):
    """Base exception class"""

    pass


class ConfigException(SITError):
    """Exceptions for invalid or unsupported configuration, including IPC method and HDL"""

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
