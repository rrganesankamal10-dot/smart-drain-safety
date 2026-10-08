import csv
import os
import unittest

from threshold_logic import evaluate

DATA = os.path.join(os.path.dirname(__file__), "..", "data", "sample_readings.csv")


class TestEvaluate(unittest.TestCase):
    def test_normal(self):
        self.assertEqual(evaluate(30, 5.0, 500), "NORMAL")

    def test_gas_alert_has_priority(self):
        self.assertEqual(evaluate(90, 0.0, 2500), "GAS_ALERT")

    def test_overflow_alert(self):
        self.assertEqual(evaluate(90, 6.0, 500), "OVERFLOW_ALERT")

    def test_blockage_warning(self):
        self.assertEqual(evaluate(70, 0.5, 500), "BLOCKAGE_WARNING")

    def test_high_level_with_good_flow_is_normal(self):
        self.assertEqual(evaluate(70, 4.0, 500), "NORMAL")

    def test_boundaries(self):
        self.assertEqual(evaluate(85, 5.0, 500), "OVERFLOW_ALERT")
        self.assertEqual(evaluate(60, 1.0, 500), "BLOCKAGE_WARNING")
        self.assertEqual(evaluate(60, 1.1, 500), "NORMAL")
        self.assertEqual(evaluate(10, 5.0, 2000), "GAS_ALERT")


class TestSampleData(unittest.TestCase):
    def test_sample_rows_match_expected_status(self):
        with open(DATA, newline="") as f:
            rows = list(csv.DictReader(f))
        self.assertGreater(len(rows), 0)
        for r in rows:
            got = evaluate(float(r["level_pct"]), float(r["flow_lpm"]), int(r["gas_raw"]))
            self.assertEqual(got, r["expected_status"], msg=str(r))


if __name__ == "__main__":
    unittest.main()
