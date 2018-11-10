import re

#NULL = "NULL"
NULL = "None"

def q(s):
    return "\"" + s + "\""

def p(s):
    return "[" + s + "]"

def sOrNull(s):
    if len(s) > 0:
        return q(s)
    return NULL

class OpCode:
    def __init__(self, name, ident):
        self.name = name
        self.lispName = ""
        self.exe = ""
        self.ident = int(ident)
        self.mode = ""

    def dump(self):
        name = "{:<18}".format(p(self.name)+" =")

        #print("  " + name + "{ " + self.exe +", " + sOrNull(self.lispName) +  " }, // " + str(self.ident))
        print(" [ "+ (", ".join([q(self.name),
                                 sOrNull(self.lispName),
                                 str(self.ident),
                                 sOrNull(self.exe),
                                 sOrNull(self.mode)])) + " ],")



re_def = re.compile("([A-Z0-9_]+)\s+([0-9]+)")
re_mk  = re.compile("mk_(proc|syntax)\((.*), \"(.*)\"\)\;")
re_op  = re.compile("(opexe_\d),\s+/\* (.*) \*/")

codes = []
codemap= {}


opexe = 0

for line in open("miniscm.c").readlines():
    line = line.strip()
    if line.startswith("#define"):
        cmd = line[7:].strip()
        m = re_def.match(cmd)
        if m:
            name = m.group(1)
            if name.startswith("OP_"):
                oc = OpCode(name, m.group(2))
                codes.append(oc)
                codemap[name] = oc
                #print(m.group(1), m.group(2))
        else:
            print cmd

    elif line.startswith("mk_proc") or line.startswith("mk_syntax"):
        #print(line)
        m = re_mk.match(line)
        if m:
            name = m.group(2)
            codemap[name].mode  = m.group(1)
            codemap[name].lispName = m.group(3)
            #print(m.group(1), m.group(2), m.group(3))

    elif line.startswith("opexe_"):
        #print(line)
        m = re_op.match(line)
        if m:
            name = m.group(2)
            if name.endswith(" = 0,"):
                name = name[:-5]
            elif name.endswith(","):
                name = name[:-1]
                
            codemap[name].exe = m.group(1)
            #print(m.group(1), m.group(2))
        
for code in codes:
    code.dump()
