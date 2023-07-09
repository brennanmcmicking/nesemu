from bs4 import BeautifulSoup
import requests

resp = requests.get("https://www.nesdev.org/obelisk-6502-guide/reference.html")

soup = BeautifulSoup(resp.text, "html.parser")

body = soup.select_one("body")
main = body.contents[9]

# cycle_count = []

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

for title in soup.select("h3"):
    # title =
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

        # print(f"case 0x{opcode:02X}:" + "{")
        # print(f"// {mnem}, {mode}, {byte_count} bytes, {cycles}")
        # print("// TODO")
        # print("break;")
        # print("}")

        # print(f"{mnem=} {mode=} {opcode=} {byte_count=} {cycles=}")
