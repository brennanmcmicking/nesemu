from bs4 import BeautifulSoup
import requests

resp = requests.get("https://www.nesdev.org/obelisk-6502-guide/reference.html")

soup = BeautifulSoup(resp.text, "html.parser")

body = soup.select_one("body")
main = body.contents[9]

# cycle_count = []

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

        # print(f"case 0x{opcode:02X}:" + "{")
        # print(f"// {mnem}, {mode}, {byte_count} bytes, {cycles}")
        # print("// TODO")
        # print("break;")
        # print("}")

        # print(f"{mnem=} {mode=} {opcode=} {byte_count=} {cycles=}")


# print("enum OpCode : uint8_t {")
# for code in opcodes:
#     code = opcodes[code]
#     print(f"{code['name']} = 0x{code['opcode']:02X},")
# print("};")

# print("uint8_t instruction_byte_count[256] = {")
byte_count = []
for i in range(256):
    if i in opcodes:
        byte_count.append(opcodes[i]["byte_count"])
    else:
        byte_count.append(0)

print(",".join(map(str, byte_count)))
# for code in sorted(opcodes):
#     code = opcodes[code]
#     print(f"[{code['opcode']}] = {code['byte_count']}, // {code['name']}")
# print("};")
