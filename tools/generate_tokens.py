#!/usr/bin/env python3
"""Generate Qt token tables from the pinned Két Design System tokens.css.

Reads third_party/ket-design-system/tokens.css and writes:
  tokens/ket-tokens.json                  resolved light and dark values (review artefact)
  src/tokens/generated/KetTokenTable.inc  the C++ table KetQt::Tokens compiles in
  src/tokens/generated/KetTokenSource.inc the source label, as a string literal

Only desktop values are taken: the base `:root` block and the plain
`[data-kv-design-system]` block of the `ket.theme` layer. Media queries, density
variants and data-theme selectors are ignored. `light-dark(a, b)` picks a branch per
theme, `var()` is substituted, `rem` becomes px (16 px root) and `calc(a / b)` is
evaluated. Run with --check to fail when the committed outputs are stale.
"""
import argparse
import hashlib
import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
SOURCE = ROOT / "third_party/ket-design-system/tokens.css"
SOURCE_INFO = ROOT / "third_party/ket-design-system/SOURCE.json"
JSON_OUT = ROOT / "tokens/ket-tokens.json"
INC_OUT = ROOT / "src/tokens/generated/KetTokenTable.inc"
SOURCE_OUT = ROOT / "src/tokens/generated/KetTokenSource.inc"
ROOT_PX = 16.0
THEMES = ("light", "dark")
NAMED_COLORS = {"transparent": "#00000000", "white": "#ffffff", "black": "#000000"}


def strip_comments(text):
    return re.sub(r"/\*.*?\*/", "", text, flags=re.S)


def blocks(text):
    """Yield (selector, body, inside_media) for every innermost rule block."""
    stack, start, out = [], 0, []
    for i, ch in enumerate(text):
        if ch == "{":
            stack.append((text[start:i].strip(), i + 1))
            start = i + 1
        elif ch == "}":
            selector, body_start = stack.pop()
            body = text[body_start:i]
            if "{" not in body:
                media = any(s.startswith("@media") for s, _ in stack)
                out.append((selector, body, media))
            start = i + 1
        elif ch == ";" and not stack:
            start = i + 1
    return out


def declarations(text):
    decls = {}
    for selector, body, media in blocks(text):
        if media or selector not in (":root", "[data-kv-design-system]"):
            continue
        for name, value in re.findall(r"--kv-([a-z0-9-]+)\s*:\s*([^;]+);", body):
            decls[name] = " ".join(value.split())
    return decls


def split_args(inner):
    depth, parts, current = 0, [], ""
    for ch in inner:
        if ch == "(":
            depth += 1
        elif ch == ")":
            depth -= 1
        if ch == "," and depth == 0:
            parts.append(current.strip())
            current = ""
        else:
            current += ch
    parts.append(current.strip())
    return parts


def replace_calls(value, function, handler):
    """Replace every `function(...)` call, innermost-safe, using handler(args)."""
    pattern = function + "("
    while True:
        at = value.find(pattern)
        if at < 0:
            return value
        depth, i = 0, at + len(function)
        for i in range(at + len(function), len(value)):
            if value[i] == "(":
                depth += 1
            elif value[i] == ")":
                depth -= 1
                if depth == 0:
                    break
        inner = value[at + len(pattern):i]
        value = value[:at] + handler(split_args(inner)) + value[i + 1:]


def resolve(name, theme, decls, seen=()):
    if name in seen:
        raise ValueError(f"cycle at --kv-{name}")
    value = decls[name]
    value = replace_calls(value, "light-dark", lambda a: a[0] if theme == "light" else a[1])

    def substitute(args):
        ref = args[0].removeprefix("--kv-")
        if ref in decls:
            return resolve(ref, theme, decls, seen + (name,))
        if len(args) > 1:
            return args[1]
        raise ValueError(f"--kv-{name} references unknown --kv-{ref}")

    value = replace_calls(value, "var", substitute)
    value = re.sub(r"(-?\d*\.?\d+)rem\b", lambda m: f"{float(m.group(1)) * ROOT_PX:g}px", value)
    value = replace_calls(value, "calc", lambda a: f"{eval_division(a[0]):g}")
    return normalize_colors(value)


def eval_division(expression):
    match = re.fullmatch(r"\s*(-?[\d.]+)\s*/\s*(-?[\d.]+)\s*", expression)
    if not match:
        raise ValueError(f"unsupported calc({expression})")
    return round(float(match.group(1)) / float(match.group(2)), 6)


def channel(text):
    text = text.strip()
    return float(text[:-1]) / 100 if text.endswith("%") else float(text)


def rgb_to_hex(args):
    joined = " ".join(args).replace(",", " ")
    color, _, alpha = joined.partition("/")
    r, g, b = (int(round(float(c))) for c in color.split())
    a = channel(alpha) if alpha.strip() else 1.0
    return qt_hex(r, g, b, a)


def qt_hex(r, g, b, a=1.0):
    if a >= 1.0:
        return f"#{r:02x}{g:02x}{b:02x}"
    return f"#{int(round(a * 255)):02x}{r:02x}{g:02x}{b:02x}"  # QColor #AARRGGBB


def normalize_colors(value):
    value = replace_calls(value, "rgba", rgb_to_hex)
    value = replace_calls(value, "rgb", rgb_to_hex)
    value = re.sub(r"#([0-9a-fA-F]{3})\b", lambda m: "#" + "".join(c * 2 for c in m.group(1)), value)
    return value.lower() if re.fullmatch(r"#[0-9a-fA-F]{6,8}", value) else value


def kind_of(light, dark):
    sample = light
    if re.fullmatch(r"#[0-9a-f]{6}([0-9a-f]{2})?", sample) or sample in NAMED_COLORS:
        return "Color"
    if re.fullmatch(r"-?[\d.]+px", sample):
        return "Length"
    if re.fullmatch(r"-?[\d.]+m?s", sample):
        return "Duration"
    if re.fullmatch(r"-?[\d.]+", sample):
        return "Number"
    if sample.startswith("cubic-bezier("):
        return "Easing"
    if re.search(r"\d+px.*#[0-9a-f]{6}", sample):
        return "Shadow"
    if '"' in sample or "sans-serif" in sample or "monospace" in sample:
        return "Font"
    return "Other"


def canonical(kind, value):
    if kind == "Color" and value in NAMED_COLORS:
        return NAMED_COLORS[value]
    if kind == "Duration" and value.endswith("ms"):
        return f"{float(value[:-2]):g}"
    if kind == "Duration":
        return f"{float(value[:-1]) * 1000:g}"
    if kind == "Length":
        return f"{float(value[:-2]):g}"
    return value


def build():
    raw = SOURCE.read_bytes()
    info = json.loads(SOURCE_INFO.read_text(encoding="utf-8"))
    digest = hashlib.sha256(raw).hexdigest()
    if digest != info["sha256"]:
        raise SystemExit(f"tokens.css sha256 {digest} does not match SOURCE.json {info['sha256']}")
    decls = declarations(strip_comments(raw.decode("utf-8")))
    tokens = {}
    for name in sorted(decls):
        light, dark = (resolve(name, theme, decls) for theme in THEMES)
        kind = kind_of(light, dark)
        tokens[name] = {"kind": kind, "light": canonical(kind, light), "dark": canonical(kind, dark)}
    document = {"source": {k: info[k] for k in ("package", "version", "revision", "sha256")}, "tokens": tokens}
    json_text = json.dumps(document, ensure_ascii=False, indent=2) + "\n"
    rows = [f"// Generated by tools/generate_tokens.py from {info['package']} {info['version']} "
            f"({info['revision'][:8]}). Do not edit.",
            f"// Source sha256 {digest}"]
    for name, token in tokens.items():
        rows.append(f'{{u"{name}", TokenKind::{token["kind"]}, u"{cpp(token["light"])}", u"{cpp(token["dark"])}"}},')
    label = f'u"{info["package"]} {info["version"]} ({info["revision"][:8]})"\n'
    return json_text, "\n".join(rows) + "\n", label


def cpp(value):
    return value.replace("\\", "\\\\").replace('"', '\\"')


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--check", action="store_true", help="fail when committed outputs are stale")
    args = parser.parse_args()
    json_text, inc_text, label = build()
    outputs = ((JSON_OUT, json_text), (INC_OUT, inc_text), (SOURCE_OUT, label))
    if args.check:
        stale = [str(p.relative_to(ROOT)) for p, text in outputs
                 if not p.exists() or p.read_text(encoding="utf-8") != text]
        if stale:
            print("stale generated files: " + ", ".join(stale), file=sys.stderr)
            return 1
        return 0
    for path, text in outputs:
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(text, encoding="utf-8")
    return 0


if __name__ == "__main__":
    sys.exit(main())
