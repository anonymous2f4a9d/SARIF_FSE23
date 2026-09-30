import unittest

from tools.export_rsf import render


def architecture(names: list[list[str]]) -> dict[str, object]:
    return {
        "@schemaVersion": "1.0",
        "structure": [
            {
                "@type": "group",
                "name": str(i),
                "nested": [{"@type": "item", "name": name} for name in members],
            }
            for i, members in enumerate(names)
        ],
    }


class ExportTests(unittest.TestCase):
    def test_preserves_full_paths_and_stable_order(self) -> None:
        source = architecture([["b/input.c", "a/input.c"], ["lib.h"]])
        self.assertEqual(
            render(source), "contain 0 a/input.c\ncontain 0 b/input.c\ncontain 1 lib.h\n"
        )

    def test_duplicate_entity_is_rejected(self) -> None:
        with self.assertRaisesRegex(ValueError, "multiple clusters"):
            render(architecture([["a.c"], ["a.c"]]))

    def test_whitespace_cannot_change_rsf_columns(self) -> None:
        with self.assertRaisesRegex(ValueError, "whitespace"):
            render(architecture([["path with space.c"]]))

    def test_empty_cluster_is_rejected(self) -> None:
        with self.assertRaisesRegex(ValueError, "Empty cluster"):
            render(architecture([[]]))

    def test_unknown_schema_is_rejected(self) -> None:
        with self.assertRaisesRegex(ValueError, "version"):
            render({"@schemaVersion": "2.0", "structure": []})

    def test_nested_group_is_rejected(self) -> None:
        with self.assertRaisesRegex(ValueError, "Nested"):
            render(
                {
                    "@schemaVersion": "1.0",
                    "structure": [
                        {
                            "@type": "group",
                            "name": "0",
                            "nested": [
                                {"@type": "group", "name": "nested", "nested": []},
                            ],
                        },
                    ],
                }
            )


if __name__ == "__main__":
    unittest.main()
