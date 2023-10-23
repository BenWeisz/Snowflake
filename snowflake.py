from typing import List
from enum import Enum
import sys

# Ripped this from stackoverflow, I forgot the link :(
class ExtendedEnum(Enum):
    @classmethod
    def list(cls):
        return list(map(lambda c: c.value, cls))

    @classmethod
    def get(cls, name):
        return list(filter(lambda c: c.value == name, cls))[0]


class SnowflakeFieldType(ExtendedEnum):
    BLANK = "BLANK"
    U32 = "U32"
    U64 = "U64"
    I32 = "I32"
    I64 = "I64"
    F32 = "F32"
    F64 = "F64"
    STR = "STR"
    ARRAY = "ARRAY"
    SNOWFLAKE = "SNOWFLAKE"

    @classmethod
    def get_ctype(cls, type):
        if type == cls.U32:
            return "unsigned int"
        elif type == cls.U64:
            return "unsigned long"
        elif type == cls.I32:
            return "int"
        elif type == cls.I64:
            return "long"
        elif type == cls.F32:
            return "float"
        elif type == cls.F64:
            return "double"
        elif type == cls.STR:
            return "char"
        else:
            return ""


class SnowflakeField:
    name: str
    type: SnowflakeFieldType
    specifier: str

    def __init__(self, name: str, type: SnowflakeFieldType, specifier: str) -> None:
        self.name = name
        self.type = type
        self.specifier = specifier


class Snowflake:
    name: str
    fields: List[SnowflakeField]
    should_export: bool

    def __init__(self, name: str, should_export: bool = False) -> None:
        self.name = name
        self.fields = []
        self.should_export = should_export

    def add_field(self, field: SnowflakeField) -> None:
        self.fields.append(field)


def hash(data: str) -> str:
    """ Thanks to Dan Bernstein: https://stackoverflow.com/questions/7666509/hash-function-for-string """
    hash_value = 5381
    for c in data:
        hash_value = ((hash_value << 5) + hash_value) + ord(c)
        hash_value = hash_value % 1099511628400
    return hex(hash_value)


def is_capitalized_and_syntactic(name: str) -> bool:
    """ Return true if the name is valid and syntactic. 
    >>> is_capitalized_and_syntactic("")
    >>> False
    >>> is_capitalized_and_syntactic(" "))
    >>> False
    >>> is_capitalized_and_syntactic("A"))
    >>> True
    >>> is_capitalized_and_syntactic("ASD"))
    >>> True
    >>> is_capitalized_and_syntactic("ASD_"))
    >>> True
    >>> is_capitalized_and_syntactic("A_SD"))
    >>> True
    >>> is_capitalized_and_syntactic("_"))
    >>> False
    >>> is_capitalized_and_syntactic("___"))
    >>> False
    """

    if len(name) == 0:
        return False
    elif len(name) == 1:
        return ord(name[0]) <= ord("Z") and ord(name[0]) >= ord("A")
    else:
        for chr in name:
            chr_ord = ord(chr)
            if (chr_ord > ord("Z") or chr_ord < ord("A")) and not chr == "_":
                return False

        return len(name) != name.count("_")


def is_snowflake_line(line: str) -> bool:
    """ Check if a line is a snowflake line. The name must satisfy is_capitalized_and_syntactic
    >>> is_snowflake_line("*STUDENT*")
    >>> True
    >>> is_snowflake_line("STUDENT")
    >>> True
    """
    stripped_lined = line.strip()

    if len(stripped_lined) == 0:
        return False

    if stripped_lined[0] != "*":
        return is_capitalized_and_syntactic(stripped_lined)

    if len(stripped_lined) >= 2:
        return is_capitalized_and_syntactic(stripped_lined[1:-1])
    else:
        return False


def is_snowflake_field_line(line: str) -> bool:
    """ Return if a line is a valid fieldute line.
    >>> is_snowflake_field_line("name U32 *Test*")
    >>> True
    """

    stripped_line = line.strip()
    tokens = stripped_line.split(" ")
    if len(tokens) < 3:
        return False

    has_type_token = tokens[1] in SnowflakeFieldType.list()
    has_specifier = tokens[2][0] == "*" and tokens[-1][-1] == "*"

    return has_type_token and has_specifier


def get_field_name(line: str) -> str:
    """ Return the name of the snowflake field.
    >>> get_field_name("name U32 *Test*")
    >>> "name"
    """
    stripped_line = line.strip()
    tokens = stripped_line.split(" ")
    return tokens[0]


def get_field_type(line: str) -> SnowflakeFieldType:
    """ Return the type of the snowflake field.
    >>> get_field_type("name U32 *Test*")
    >>> SnowflakeFieldType.U32
    """

    stripped_line = line.strip()
    tokens = stripped_line.split(" ")
    return SnowflakeFieldType.get(tokens[1])


def get_field_specifier(line: str) -> str:
    """ Return the specifier of the snowflake field.
    >>> get_field_specifier("name U32 *Test*")
    >>> "Test"
    """
    stripped_line = line.strip()
    tokens = stripped_line.split(" ")[2:]

    specifier = " ".join(tokens)[1:-1]
    return specifier


def load_snowflakes(path: str) -> List[Snowflake]:
    """ Load all the snowflakes from the given file. """
    snowflakes = []

    with open(path, "r") as in_file:
        lines = in_file.readlines()

        snowflake = None
        for line in lines:
            if is_snowflake_line(line):
                if snowflake is not None:
                    snowflakes.append(snowflake)

                should_export = line[0] != "*"
                name = line.strip() if should_export else line.strip()[1:-1]
                snowflake = Snowflake(name, should_export)
            elif is_snowflake_field_line(line):
                field_name = get_field_name(line)
                field_type = get_field_type(line)
                field_specifier = get_field_specifier(line)

                snowflake_field = SnowflakeField(
                    field_name, field_type, field_specifier
                )
                snowflake.add_field(snowflake_field)

        if snowflake is not None:
            snowflakes.append(snowflake)

    return snowflakes


def get_snowflake_dependencies(snowflakes: List[Snowflake]) -> List[List[Snowflake]]:
    snowflakes_dependencies = []
    for snowflake in snowflakes:
        snowflake_dependecies = []
        for field in snowflake.fields:
            if (
                field.type == SnowflakeFieldType.ARRAY
                or field.type == SnowflakeFieldType.SNOWFLAKE
            ):
                for other_snowflake in snowflakes:
                    if other_snowflake.name == field.specifier:
                        snowflake_dependecies.append(other_snowflake)
                        break

        snowflakes_dependencies.append(snowflake_dependecies)

    return snowflakes_dependencies


def topo_sort(snowflakes: List[Snowflake]) -> List[Snowflake]:
    topo_order = []

    dependecies = get_snowflake_dependencies(snowflakes)
    visited = [False] * len(snowflakes)
    while len(topo_order) != len(snowflakes):
        # Find the snowflake with the least amount of deps
        min_deps_snowflake = None
        min_deps = len(snowflakes) + 1
        min_deps_i = -1
        for snowflake_i, snowflake in enumerate(snowflakes):
            if not visited[snowflake_i] and len(dependecies[snowflake_i]) < min_deps:
                min_deps = len(dependecies[snowflake_i])
                min_deps_snowflake = snowflake
                min_deps_i = snowflake_i

        # Remove snowflake
        topo_order.append(min_deps_snowflake)
        visited[min_deps_i] = True

        new_dependencies = []
        for snowflake_deps in dependecies:
            new_snowflake_deps = list(
                filter(lambda x: x.name != min_deps_snowflake.name, snowflake_deps)
            )
            new_dependencies.append(new_snowflake_deps)

        dependecies = new_dependencies

    return topo_order


def generate_structs(snowflakes: List[Snowflake]) -> str:
    topo_order_snowflakes = topo_sort(snowflakes)
    struct_str = ""
    for snowflake in topo_order_snowflakes:
        struct_str += f"typedef struct {snowflake.name}" + " {\n"
        for snowflake_field in snowflake.fields:
            if snowflake_field.type == SnowflakeFieldType.STR:
                struct_str += (
                    f"    char {snowflake_field.name}[{snowflake_field.specifier}];\n"
                )
            elif snowflake_field.type == SnowflakeFieldType.ARRAY:
                struct_str += (
                    f"    {snowflake_field.specifier}* {snowflake_field.name};\n"
                )
                struct_str += f"    long {snowflake_field.name}_len;\n"
            elif snowflake_field.type == SnowflakeFieldType.SNOWFLAKE:
                struct_str += (
                    f"    {snowflake_field.specifier} {snowflake_field.name};\n"
                )
            else:
                struct_str += "    " + SnowflakeFieldType.get_ctype(
                    snowflake_field.type
                )
                struct_str += f" {snowflake_field.name};\n"
        struct_str += "} " + f"{snowflake.name};\n\n"

    return struct_str


def print_usage_message():
    print("\nUsage:")
    print("- There must be at most 2 arguments, the last must be a path to a .sf file.")
    print(
        "- If you only want to get the hash of the .sf file and not generate the IO code, make the first parameter -h or --hash."
    )


if __name__ == "__main__":
    args = sys.argv[1:]
    if len(args) == 0:
        print("ERROR: No arguments provided")
        print_usage_message()
        exit()
    elif not args[-1].endswith(".sf"):
        print("ERROR: The final argument must be the path to an .sf file")
        exit()
    elif len(args) == 2 and (args[0] == "-h" or args[0] == "--hash"):
        snowflakes = load_snowflakes(args[-1])
        struct_str = generate_structs(snowflakes)
        print(f"The hash for .sf file {args[-1]} is: {hash(struct_str)}")
    elif args[-1].endswith(".sf"):
        snowflakes = load_snowflakes(args[-1])
        struct_str = generate_structs(snowflakes)
    else:
        print("ERROR: The arguments you passed are malformed.")
        print_usage_message()
        exit()

    struct_str = generate_structs(snowflakes)
    print(struct_str)
