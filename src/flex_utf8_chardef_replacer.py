"""
Simple script that replaces INSERT_CHARACTER_DEFS in file given in third argument
with hexadecimal bytes of latin characters that are given by two file (first and
second argument)
"""
import sys

def read_hex_bytes(f):
    onebytes = []
    twobytes = []
    threebytes = []
    for l in f:
        l = l.strip()
        if len(l) == 2:
            onebytes.append(l)
        elif len(l) == 4:
            twobytes.append(l)
        elif len(l) == 6:
            threebytes.append(l)
    return onebytes, twobytes, threebytes

def to_flex(char_class_name, characters):
    result = ""
    l = len(characters[0])
    pos = 0
    line_num = 0
    result += char_class_name + "_" + str(line_num) + " (" 
    first = True
    for char in characters:
        if not first:
            result += "|"
        first = False
        for charbytepos in xrange(l/2):
            charhex = char[charbytepos*2:charbytepos*2+2]
            result += "\\x" + charhex
            pos += 4
        if pos >= 1024:
            result += ")\n"
            pos = 0
            line_num += 1
            result += char_class_name + "_" + str(line_num) + " (" 
            first = True
    result += ")\n"
    result += char_class_name + " (" + "|".join("{" + char_class_name + "_" + str(i) + "}" for i in xrange(line_num+1)) + ")\n"
    return result

def main():
    small_one, small_two, small_three = read_hex_bytes(file(sys.argv[1]))
    capital_one, capital_two, capital_three = read_hex_bytes(file(sys.argv[2]))

    flex_file = file(sys.argv[3])
    flex_data = flex_file.read()
    s = ""
    s += to_flex("LATIN_SMALL_1_BYTE", small_one)
    s += to_flex("LATIN_CAPITAL_1_BYTE", capital_one)
    s += to_flex("LATIN_SMALL_2_BYTE", small_two)
    s += to_flex("LATIN_CAPITAL_2_BYTE", capital_two)
    s += to_flex("LATIN_SMALL_3_BYTE", small_three)
    s += to_flex("LATIN_CAPITAL_3_BYTE", capital_three)
    s += "LATIN_CHARACTER_SMALL ({LATIN_SMALL_1_BYTE}|{LATIN_SMALL_2_BYTE}|{LATIN_SMALL_3_BYTE})\n"
    s += "LATIN_CHARACTER_CAPITAL ({LATIN_CAPITAL_1_BYTE}|{LATIN_CAPITAL_2_BYTE}|{LATIN_CAPITAL_3_BYTE})\n"
    s += "LATIN_CHAR ({LATIN_CHARACTER_SMALL}|{LATIN_CHARACTER_CAPITAL})\n"
    flex_data = flex_data.replace("INSERT_CHARACTER_DEFS", s)
    sys.stdout.write(flex_data)

if __name__ == "__main__":
    main()

