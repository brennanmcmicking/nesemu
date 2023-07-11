from bs4 import BeautifulSoup
import requests

text = None

try:
    with open("cached.html") as f:
        text = f.read()
except FileNotFoundError:
    pass

if text is None:
    resp = requests.get("https://www.nesdev.org/obelisk-6502-guide/reference.html")
    text = resp.text
    with open("cached.html", "w") as f:
        f.write(text)

soup = BeautifulSoup(text, "html.parser")

body = soup.select_one("body")
main = body.contents[9]

from collections import Counter

cycle_count = Counter()

kImplied = 0
kAccumulator = 1
kImmediate = 2
kZeroPage = 3
kZeroPageX = 4
kZeroPageY = 5
kRelative = 6
kAbsolute = 7
kAbsoluteX = 8
kAbsoluteY = 9
kIndirect = 10
kIndexedIndirect = 11
kIndirectIndexed = 12

opcodes = []

modes = set()

modeVals = {
    "Absolute": kAbsolute,
    "Absolute,X": kAbsoluteX,
    "Absolute,Y": kAbsoluteY,
    "Accumulator": kAccumulator,
    "Immediate": kImmediate,
    "Implied": kImplied,
    "Indirect": kIndirect,
    "(Indirect,X)": kIndexedIndirect,
    "(Indirect),Y": kIndirectIndexed,
    "Relative": kRelative,
    "Zero Page": kZeroPage,
    "Zero Page,X": kZeroPageX,
    "Zero Page,Y": kZeroPageY,
}

"""
enum AddrMode {
  // 8-bit (value)
  kAccumulator,  // A
  kImmediate,    // #$00
  // 16-bit (address)
  kZeroPage,         // $00
  kZeroPageX,        // $00, X (zero page wraparound)
  kZeroPageY,        // $00, Y (zero page wraparound)
  kRelative,         // $0000 (signed)
  kAbsolute,         // $0000
  kAbsoluteX,        // $0000, X
  kAbsoluteY,        // $0000, Y
  kIndirect,         // ($0000)
  kIndexedIndirect,  // ($00, X) (zero page wraparound)
  kIndirectIndexed,  // ($00), Y
};
"""

# this should map to the C++ enum above
modeNames = {
    kAbsolute: "kAbsolute",
    kImmediate: "kImmediate",
    kImplied: "UNIMPLEMENTED",
    kIndirect: "kIndirect",
    kIndexedIndirect: "kIndexedIndirect",
    kIndirectIndexed: "kIndirectIndexed",
    kRelative: "kRelative",
    kZeroPage: "kZeroPage",
    kZeroPageX: "kZeroPageX",
    kZeroPageY: "kZeroPageY",
    kAbsoluteX: "kAbsoluteX",
    kAbsoluteY: "kAbsoluteY",
    kAccumulator: "kAccumulator",
}

opcodes = {}

for title in soup.select("h3"):
    table = title.find_next("table").find_next("table")
    mnem = title.get_text().split("-")[0].strip()
    rows = table.select("tr")[1:]  # skip header
    for row in rows:
        # print(row.select("td"))
        mode, opcode, byte_count, cycles = map(
            lambda x: x.get_text().strip(), row.select("td")
        )
        byte_count = " ".join(byte_count.split())
        cycles = " ".join(cycles.split())
        mode = " ".join(mode.split())
        opcode = int(opcode.strip("$"), 16)

        cycle_count.update([(cycles[2:], mode)])

        modes.add(mode)
        mode = modeVals[mode]

        name = "k"
        name += mnem
        name += (
            {
                kImplied: "",
                kAccumulator: "_A",
                kImmediate: "_IMM",
                kZeroPage: "_ZP",
                kZeroPageX: "_ZPX",
                kZeroPageY: "_ZPY",
                kRelative: "_REL",
                kAbsolute: "_ABS",
                kAbsoluteX: "_ABSX",
                kAbsoluteY: "_ABSY",
                kIndirect: "_IND",
                kIndexedIndirect: "_INDX",
                kIndirectIndexed: "_INDY",
            }
        )[mode]

        opcodes[opcode] = {
            "mnem": mnem,
            "mode": mode,
            "opcode": opcode,
            "byte_count": byte_count,
            "cycles": cycles,
            "name": name,
        }
        # if mode == kAbsoluteY and cycles[2:] == "":
        #     print(mnem)

        # print(f"case 0x{opcode:02X}:" + "{")
        # print(f"// {mnem}, {mode}, {byte_count} bytes, {cycles}")
        # print("// TODO")
        # print("break;")
        # print("}")

        # print(f"{mnem=} {mode=} {opcode=} {byte_count=} {cycles=}")

# # print(cycle_count)
# from collections import defaultdict

# mode_types = defaultdict(set)
# for cycle_mode, mode in cycle_count:
#     # print(f"{cycle_mode=} {mode=}")
#     # mode_types[mode].add(cycle_mode)
#     mode_types[cycle_mode].add(mode)
# # print(mode_types)

# for k in sorted(mode_types):
#     print(f"{k=}: {mode_types[k]=}")

# print cycle counts
# {'', '(+1 if branch succeeds +2 if to a new page)', '(+1 if page crossed)'}

# k='': mode_types[k]={'(Indirect),Y', 'Absolute,X', 'Implied', 'Zero Page,X', 'Indirect', 'Immediate', 'Zero Page,Y', 'Accumulator', '(Indirect,X)', 'Absolute,Y', 'Absolute', 'Zero Page'}
# k='(+1 if branch succeeds +2 if to a new page)': mode_types[k]={'Relative'}
# k='(+1 if page crossed)': mode_types[k]={'(Indirect),Y', 'Absolute,Y', 'Absolute,X'}

# print("switch (opcode) {")
# for code in opcodes:
#     code = opcodes[code]
#     cycles = code["cycles"]
#     mode = code["mode"]
#     base_cycles, cycle_type = int(cycles[0]), cycles[2:]
#     if cycle_type == "":
#         print(f"case {code['name']}: return {base_cycles};")
#     elif cycle_type == "(+1 if page crossed)":
#         print(f"case {code['name']}: " + "{ // indexed addressing instruction")
#         base_addr = None
#         index_addr = None
#         if mode == kAbsoluteX or mode == kAbsoluteY:
#             base_addr = kAbsolute
#             index_addr = mode
#         elif mode == kIndirectIndexed:  # indirect indexed but not indexed indirect
#             base_addr = kIndirect
#             index_addr = mode
#         else:
#             raise Exception(f"unhandled mode for cycle timing {mode} on {code['name']}")
#         print(f"uint16_t base_addr = addr_fetch({modeNames[base_addr]});")
#         print(f"uint16_t index_addr = addr_fetch({modeNames[index_addr]});")
#         print(f"std::size_t cycles = {base_cycles};")
#         print(f"if (crossed_page(base_addr, index_addr)) cycles += 1;")
#         print("return cycles;")
#         print("}")
#     elif cycle_type == "(+1 if branch succeeds +2 if to a new page)":
#         print(f"case {code['name']}: " + "{ // branching instruction")
#         print(f"std::size_t cycles = {base_cycles};")
#         print(f"/* TODO fill in the function for {code['name']}")
#         print(f"uint16_t base_addr = PC_;")
#         print(f"uint16_t branch_addr = addr_fetch({modeNames[kRelative]});")
#         print(f"if (!BRANCH_INSTRUCTION()) return cycles;")
#         print(f"cycles += 1;")
#         print(f"if (crossed_page(base_addr, branch_addr)) cycles += 1;")
#         print(f"*/")
#         print(f"return cycles;")
#         print("}")
# print("}")

#     # print(f"{code['name']}, // {cycle_type}")
#     # print("case 0x{opcode:02X}:" + "{")


# print("enum OpCode : uint8_t {")
# for code in opcodes:
#     code = opcodes[code]
#     print(f"{code['name']} = 0x{code['opcode']:02X},")
# print("};")

# print("uint8_t instruction_byte_count[256] = {")
# byte_count = []
# for i in range(256):
#     if i in opcodes:
#         byte_count.append(opcodes[i]["byte_count"])
#     else:
#         byte_count.append(0)
# print(",".join(map(str, byte_count)))


# for code in sorted(opcodes):
#     code = opcodes[code]
#     print(f"[{code['opcode']}] = {code['byte_count']}, // {code['name']}")
# print("};")
