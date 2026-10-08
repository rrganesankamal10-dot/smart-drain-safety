"""Reference implementation of the alert rules used in firmware/smart_drain/smart_drain.ino.

Keep the thresholds here identical to firmware/smart_drain/config.h.
"""

LEVEL_WARN_PCT = 60.0
LEVEL_ALERT_PCT = 85.0
FLOW_LOW_LPM = 1.0
GAS_ALERT_RAW = 2000


def evaluate(level_pct: float, flow_lpm: float, gas_raw: int) -> str:
    """Return the system status for one set of sensor readings."""
    if gas_raw >= GAS_ALERT_RAW:
        return "GAS_ALERT"
    if level_pct >= LEVEL_ALERT_PCT:
        return "OVERFLOW_ALERT"
    if level_pct >= LEVEL_WARN_PCT and flow_lpm <= FLOW_LOW_LPM:
        return "BLOCKAGE_WARNING"
    return "NORMAL"
