"""Export a flat SARIF cluster_result.json to ARCADE-compatible contain RSF.

Entity paths are preserved verbatim. Validate the complete partition before
writing, so duplicate entities or whitespace cannot silently corrupt evaluation.
"""

from __future__ import annotations

import argparse
import json
from pathlib import Path


def obj(value: object) -> dict[str, object]:
    if not isinstance(value, dict) or not all(isinstance(k, str) for k in value):
        raise ValueError("Expected a JSON object")
    return dict(value)


def items(value: object) -> list[object]:
    if not isinstance(value, list):
        raise ValueError("Expected a JSON array")
    return list(value)


def token(value: object) -> str:
    if not isinstance(value, str) or not value or any(c.isspace() for c in value):
        raise ValueError("RSF identifiers must be nonempty strings without whitespace")
    return value


def assignments(payload: object) -> list[tuple[str, str]]:
    root = obj(payload)
    if root.get("@schemaVersion") != "1.0":
        raise ValueError("Expected cluster schema version 1.0")
    rows: list[tuple[str, str]] = []
    seen_entities: set[str] = set()
    seen_groups: set[str] = set()
    for raw in items(root["structure"]):
        group = obj(raw)
        if group.get("@type") != "group":
            raise ValueError("Expected flat groups")
        name = token(group["name"])
        if name in seen_groups:
            raise ValueError(f"Duplicate cluster name: {name}")
        seen_groups.add(name)
        members = items(group["nested"])
        if not members:
            raise ValueError(f"Empty cluster: {name}")
        for raw_member in members:
            member = obj(raw_member)
            if member.get("@type") != "item":
                raise ValueError("Nested groups are not supported by the flat RSF exporter")
            entity = token(member["name"])
            if entity in seen_entities:
                raise ValueError(f"Entity belongs to multiple clusters: {entity}")
            seen_entities.add(entity)
            rows.append((name, entity))
    if not rows:
        raise ValueError("Empty architecture")
    return sorted(rows)


def render(payload: object) -> str:
    return "".join(f"contain {cluster} {entity}\n" for cluster, entity in assignments(payload))


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--input", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    try:
        rsf = render(json.loads(args.input.read_text(encoding="utf-8")))
    except (ValueError, KeyError) as error:
        parser.exit(1, f"Invalid architecture: {error}\n")
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(rsf, encoding="utf-8")


if __name__ == "__main__":
    main()
