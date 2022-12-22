import re


class TemplateRenderer:
    def __init__(self):

        self.variable_re = re.compile(
            r"\{\{(.*?)\}\}", re.IGNORECASE | re.MULTILINE
        )

    def render(self, template, mapping):

        return self.variable_re.sub(
            lambda m: str(mapping.get(m.group(1), m.group(0))), template
        )
