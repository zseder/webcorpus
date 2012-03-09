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

nolangmsg = """WARNING: Creating sentence tokenizer without language specific 
nonbreaking prefixes. Sentence tokenizer will be rebuilt online, 
i.e. before tokenizing, where language is already known\n"""
filenotfoundmsg = """WARNING: Creating sentence tokenizer without language specific 
nonbreaking prefixes because file %s not found\n""" 
def main():
    small_one, small_two, small_three = read_hex_bytes(file(sys.argv[1]))
    capital_one, capital_two, capital_three = read_hex_bytes(file(sys.argv[2]))
    if(sys.argv[3].endswith("undefined.nbp")):
        sys.stderr.write(nolangmsg)
        nonbreaking_prefixes = None
    else:
        try:
            nonbreaking_prefixes = [w.strip() for w in file(sys.argv[3]).read().split("\n") if len(w.strip()) > 0]
        except IOError:
            sys.stderr.write(filenotfoundmsg % sys.argv[3])
            nonbreaking_prefixes = None

    flex_file = file(sys.argv[4])
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
    if nonbreaking_prefixes is not None:
        flex_data = flex_data.replace("INSERT_LANGUAGE_NONBREAKING_PREFIX", "|\"" + "\"|\"".join(nonbreaking_prefixes) + "\"")
    else:
        flex_data = flex_data.replace("INSERT_LANGUAGE_NONBREAKING_PREFIX", "")

    sys.stdout.write(flex_data)

if __name__ == "__main__":
    main()

