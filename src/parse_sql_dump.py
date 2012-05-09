import sys

for line in sys.stdin:
    if not line.startswith("INSERT INTO"):
        continue
    
    line = line.decode("utf-8", "ignore").split("VALUES", 1)[1].strip().rstrip(";")
    entries = eval("( %s )" % line)
    for entry in entries:
        entry_elements = list(entry)
        entry_elements = [str(ee) if type(ee) != str else ee.decode("utf-8") for ee in entry_elements]
        try:
            print "\t".join([ee for ee in entry_elements]).encode("utf-8")
        except UnicodeDecodeError, ude:
            sys.stderr.write("There is an encoding problem with a link:\n")
            sys.stderr.write(str(entry).decode("utf-8", "ignore") + "\n")
