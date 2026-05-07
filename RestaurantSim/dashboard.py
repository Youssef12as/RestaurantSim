"""
Restaurant Simulation  Analytics Dashboard
====================================================
Single-file Streamlit dashboard that reads `restaurant_data.csv`
exported by the C++ Restaurant Management Simulation and renders
professional, academic-grade visualisations.

Run:  streamlit run dashboard.py
"""

import streamlit as st
import pandas as pd
import plotly.express as px
import plotly.graph_objects as go
import os, sys

# ──────────────────────────────────────────────────────────────────────
# 0. PAGE CONFIG & THEME
# ──────────────────────────────────────────────────────────────────────
st.set_page_config(
    page_title=" Restaurant Analytics",
    page_icon="📊",
    layout="wide",
    initial_sidebar_state="collapsed",
)

# Matte charcoal palette — injected once via custom CSS
_CSS = """
<style>
@import url('https://fonts.googleapis.com/css2?family=Inter:wght@300;400;500;600;700&display=swap');

/* ── Root overrides ── */
html, body, [class*="css"] {
    font-family: 'Inter', sans-serif;
}

/* Main background */
.stApp {
    background: linear-gradient(175deg, #0f0f0f 0%, #1a1a1a 50%, #141414 100%);
}

/* Sidebar */
section[data-testid="stSidebar"] {
    background-color: #141414;
}

/* Metric cards */
div[data-testid="stMetric"] {
    background: linear-gradient(135deg, #1c1c1c 0%, #242424 100%);
    border: 1px solid #2a2a2a;
    border-radius: 12px;
    padding: 18px 22px;
    transition: transform 0.2s ease, box-shadow 0.2s ease;
}
div[data-testid="stMetric"]:hover {
    transform: translateY(-2px);
    box-shadow: 0 6px 20px rgba(0, 0, 0, 0.45);
}
div[data-testid="stMetric"] label {
    color: #8a8a8a !important;
    font-weight: 500;
    letter-spacing: 0.4px;
    text-transform: uppercase;
    font-size: 0.72rem !important;
}
div[data-testid="stMetric"] [data-testid="stMetricValue"] {
    color: #e0e0e0 !important;
    font-weight: 700;
}

/* Headings */
h1, h2, h3 {
    color: #d4d4d4 !important;
    font-weight: 600 !important;
    letter-spacing: -0.3px;
}

/* Subheader pills */
.section-header {
    background: linear-gradient(90deg, #1e1e1e, transparent);
    border-left: 3px solid #6b8fa3;
    padding: 8px 16px;
    margin: 28px 0 14px 0;
    border-radius: 0 6px 6px 0;
    color: #a0b4c0;
    font-size: 0.82rem;
    font-weight: 600;
    text-transform: uppercase;
    letter-spacing: 1px;
}

/* Recommendation cards */
.rec-card {
    background: linear-gradient(135deg, #1b2025 0%, #1e2328 100%);
    border: 1px solid #2a3038;
    border-radius: 10px;
    padding: 16px 20px;
    margin-bottom: 10px;
    transition: border-color 0.25s ease;
}
.rec-card:hover {
    border-color: #3f5060;
}
.rec-card .rec-icon {
    font-size: 1.15rem;
    margin-right: 8px;
}
.rec-card .rec-title {
    font-weight: 600;
    color: #c0cdd6;
    font-size: 0.88rem;
}
.rec-card .rec-body {
    color: #8a96a0;
    font-size: 0.8rem;
    margin-top: 4px;
    line-height: 1.55;
}

/* Dividers */
hr {
    border: none;
    border-top: 1px solid #252525;
    margin: 32px 0;
}

/* Plotly chart containers */
.stPlotlyChart {
    background: #191919;
    border: 1px solid #242424;
    border-radius: 12px;
    padding: 6px;
}

/* Hide default Streamlit branding */
#MainMenu {visibility: hidden;}
footer {visibility: hidden;}
header {visibility: hidden;}
</style>
"""
st.markdown(_CSS, unsafe_allow_html=True)

# ──────────────────────────────────────────────────────────────────────
# 1. PLOTLY DARK TEMPLATE
# ──────────────────────────────────────────────────────────────────────
_PLOTLY_LAYOUT = dict(
    paper_bgcolor="rgba(0,0,0,0)",
    plot_bgcolor="rgba(0,0,0,0)",
    font=dict(family="Inter, sans-serif", color="#9e9e9e", size=12),
    margin=dict(l=40, r=30, t=50, b=40),
    legend=dict(
        bgcolor="rgba(0,0,0,0)",
        borderwidth=0,
        font=dict(size=11, color="#888"),
    ),
    xaxis=dict(gridcolor="#252525", zerolinecolor="#303030"),
    yaxis=dict(gridcolor="#252525", zerolinecolor="#303030"),
)

# Muted pastel accent palette for chart series
_PALETTE = [
    "#6b9dae",  # soft cyan
    "#ae6b8f",  # muted magenta
    "#8fa36b",  # sage green
    "#a3896b",  # warm taupe
    "#7c6bae",  # muted lavender
    "#ae9e6b",  # muted gold
    "#6baead",  # teal
]

# Human-readable labels for order type codes
_TYPE_LABELS = {
    "ODG": "Dine-In (Gold)",
    "ODN": "Dine-In (Normal)",
    "OT":  "Takeaway",
    "OVC": "VIP Delivery (Cash)",
    "OVG": "VIP Delivery (Gold)",
    "OVN": "VIP Delivery (Normal)",
    "OC":  "Combo",
}

# ──────────────────────────────────────────────────────────────────────
# 2. DATA LOADING
# ──────────────────────────────────────────────────────────────────────
CSV_NAME = "restaurant_data.csv"

# Search in several likely locations
_SEARCH_DIRS = [
    os.path.dirname(os.path.abspath(__file__)),                   # same folder as script
    os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."),  # parent
    os.getcwd(),                                                  # cwd
]


@st.cache_data(show_spinner=False)
def load_data(path: str) -> pd.DataFrame:
    """Load and sanitise the simulation CSV."""
    df = pd.read_csv(path)
    # Strip whitespace from column names (common C++ fprintf quirk)
    df.columns = [c.strip() for c in df.columns]
    # Ensure numeric types
    for col in ["ID", "TQ", "TA", "TR", "TS", "TF", "WaitTime", "ServiceTime"]:
        if col in df.columns:
            df[col] = pd.to_numeric(df[col], errors="coerce")
    df["Type"] = df["Type"].astype(str).str.strip()
    return df


def find_csv() -> str | None:
    for d in _SEARCH_DIRS:
        candidate = os.path.join(d, CSV_NAME)
        if os.path.isfile(candidate):
            return candidate
    return None


csv_path = find_csv()

if csv_path is None:
    st.markdown("##")
    st.markdown(
        "<div style='text-align:center; margin-top: 120px;'>"
        "<span style='font-size:3rem;'>📂</span><br><br>"
        "<span style='color:#8a96a0; font-size:1.1rem; font-weight:500;'>"
        "No <code style=\"color:#6b9dae\">restaurant_data.csv</code> found in the project directory."
        "</span><br>"
        "<span style='color:#606870; font-size:0.88rem;'>"
        "Run the C++ Restaurant Simulation first to generate the data file, then refresh this page."
        "</span></div>",
        unsafe_allow_html=True,
    )
    st.stop()

df = load_data(csv_path)

# ──────────────────────────────────────────────────────────────────────
# 3. DERIVED METRICS
# ──────────────────────────────────────────────────────────────────────
total_orders     = len(df)
finished_orders  = int((df["TF"] > 0).sum()) if "TF" in df.columns else 0
health_score     = (finished_orders / total_orders * 100) if total_orders > 0 else 0.0
avg_wait         = df["WaitTime"].mean() if "WaitTime" in df.columns else 0.0
avg_service      = df["ServiceTime"].mean() if "ServiceTime" in df.columns else 0.0

type_counts      = df["Type"].value_counts()

# Per-type averages for grouped bar
type_avg = (
    df.groupby("Type")[["WaitTime", "ServiceTime"]]
    .mean()
    .reset_index()
    .sort_values("WaitTime", ascending=False)
)

# Throughput: cumulative finished orders sorted by TF
finished_df = df[df["TF"] > 0].sort_values("TF").copy()
finished_df["CumulativeFinished"] = range(1, len(finished_df) + 1)

# ──────────────────────────────────────────────────────────────────────
# 4. HEADER
# ──────────────────────────────────────────────────────────────────────
st.markdown(
    "<div style='text-align:center; padding: 18px 0 4px 0;'>"
    "<span style='font-size:1.65rem; font-weight:700; color:#d0d0d0; letter-spacing:-0.5px;'>"
    "📊&nbsp; Restaurant Analytics"
    "</span><br>"
    "<span style='font-size:0.78rem; color:#5e6870; font-weight:400;'>"
    "Post-simulation performance report&nbsp;&nbsp;·&nbsp;&nbsp;"
    f"Source: <code style='color:#6b9dae; font-size:0.76rem;'>{os.path.basename(csv_path)}</code>"
    "</span></div>",
    unsafe_allow_html=True,
)
st.markdown("---")

# ──────────────────────────────────────────────────────────────────────
# 5. KPI ROW  — Health Score gauge + summary metrics
# ──────────────────────────────────────────────────────────────────────
st.markdown('<div class="section-header">Key Performance Indicators</div>', unsafe_allow_html=True)

kpi_left, kpi_gauge, kpi_right = st.columns([1, 1.6, 1])

with kpi_left:
    st.metric("Total Orders", f"{total_orders:,}")
    st.metric("Finished Orders", f"{finished_orders:,}")

with kpi_gauge:
    # Sleek gauge using Plotly indicator
    score_color = (
        "#6b9dae" if health_score >= 85
        else "#ae9e6b" if health_score >= 70
        else "#ae6b6b"
    )
    gauge = go.Figure(
        go.Indicator(
            mode="gauge+number",
            value=health_score,
            number=dict(suffix="%", font=dict(size=40, color="#d4d4d4", family="Inter")),
            title=dict(text=" Health Score", font=dict(size=14, color="#8a8a8a")),
            gauge=dict(
                axis=dict(range=[0, 100], tickwidth=0, tickcolor="#333", dtick=25,
                          tickfont=dict(size=10, color="#555")),
                bar=dict(color=score_color, thickness=0.35),
                bgcolor="#1c1c1c",
                borderwidth=0,
                steps=[
                    dict(range=[0, 50],  color="#1e1e1e"),
                    dict(range=[50, 70], color="#222222"),
                    dict(range=[70, 85], color="#252525"),
                    dict(range=[85, 100], color="#282828"),
                ],
                threshold=dict(
                    line=dict(color="#555", width=2),
                    thickness=0.7,
                    value=health_score,
                ),
            ),
        )
    )
    gauge.update_layout(
        height=230,
        **{k: v for k, v in _PLOTLY_LAYOUT.items() if k != "xaxis" and k != "yaxis"},
    )
    st.plotly_chart(gauge, width="stretch", key="gauge")

with kpi_right:
    st.metric("Avg Wait Time", f"{avg_wait:.1f} ts")
    st.metric("Avg Service Time", f"{avg_service:.1f} ts")

st.markdown("---")

# ──────────────────────────────────────────────────────────────────────
# 6. SMART RECOMMENDATIONS ENGINE
# ──────────────────────────────────────────────────────────────────────
st.markdown('<div class="section-header">Smart Recommendations Engine</div>', unsafe_allow_html=True)


def _rec(icon: str, title: str, body: str):
    """Render a recommendation card."""
    st.markdown(
        f'<div class="rec-card">'
        f'<span class="rec-icon">{icon}</span>'
        f'<span class="rec-title">{title}</span>'
        f'<div class="rec-body">{body}</div>'
        f'</div>',
        unsafe_allow_html=True,
    )


rec_cols = st.columns(2)
recommendations: list[tuple[str, str, str]] = []

# Rule 1: High wait time → chef shortage
if avg_wait > 50:
    recommendations.append((
        "⏳", "Potential Chef Shortage",
        f"Average wait time is <b>{avg_wait:.1f}</b> time-steps, which exceeds the 50 ts threshold. "
        "Consider hiring additional chefs or optimising order assignment to reduce queueing delays.",
    ))

# Rule 2: Low health score → cancellations
if health_score < 70:
    recommendations.append((
        "⚠️", "High Cancellation / Failure Rate",
        f"Only <b>{health_score:.1f}%</b> of orders were completed. "
        "Investigate bottlenecks in the cooking pipeline or delivery logistics that may be causing "
        "orders to time-out or get cancelled.",
    ))

# Rule 3: Delivery-heavy mix
delivery_types = {"OVC", "OVG", "OVN"}
delivery_pct = df[df["Type"].isin(delivery_types)].shape[0] / max(total_orders, 1) * 100
if delivery_pct > 60:
    recommendations.append((
        "🛵", "Delivery-Dominant Order Mix",
        f"<b>{delivery_pct:.0f}%</b> of orders are deliveries. Ensure enough scooters are available "
        "and consider route optimisation to lower service times.",
    ))

# Rule 4: Service time outlier
if avg_service > 40:
    recommendations.append((
        "🕐", "Elevated Service Times",
        f"Average service time is <b>{avg_service:.1f}</b> ts. Long serving durations can cascade into "
        "table/scooter shortages. Review table turnover rates and scooter fleet speed.",
    ))

# Rule 5: Combo-heavy load
combo_pct = df[df["Type"] == "OC"].shape[0] / max(total_orders, 1) * 100
if combo_pct > 15:
    recommendations.append((
        "🍱", "Combo Order Load",
        f"<b>{combo_pct:.0f}%</b> of orders are Combos, which require multi-chef coordination. "
        "Monitor chef utilisation to prevent cascading delays.",
    ))

# Optimal fallback
if len(recommendations) == 0:
    recommendations.append((
        "✅", "System Operating Optimally",
        "All monitored metrics are within healthy thresholds. The restaurant is running efficiently "
        "with a strong health score and balanced order fulfilment.",
    ))

# Render in 2 columns
for idx, (icon, title, body) in enumerate(recommendations):
    with rec_cols[idx % 2]:
        _rec(icon, title, body)

st.markdown("---")

# ──────────────────────────────────────────────────────────────────────
# 7. VISUALISATIONS
# ──────────────────────────────────────────────────────────────────────
st.markdown('<div class="section-header">Order Distribution &amp; Time Analysis</div>', unsafe_allow_html=True)

chart_left, chart_right = st.columns([1, 1.3])

# ── 7a. Doughnut: Order-type distribution ────────────────────────────
with chart_left:
    labels = [_TYPE_LABELS.get(t, t) for t in type_counts.index]
    fig_pie = go.Figure(
        go.Pie(
            labels=labels,
            values=type_counts.values,
            hole=0.55,
            marker=dict(colors=_PALETTE[: len(labels)], line=dict(color="#1a1a1a", width=2)),
            textinfo="percent+label",
            textfont=dict(size=11, color="#b0b0b0"),
            hovertemplate="<b>%{label}</b><br>Count: %{value}<br>Share: %{percent}<extra></extra>",
        )
    )
    fig_pie.update_layout(
        title=dict(text="Order Type Distribution", font=dict(size=14, color="#a0a0a0"), x=0.5),
        showlegend=False,
        height=380,
        **{k: v for k, v in _PLOTLY_LAYOUT.items() if k not in ("xaxis", "yaxis")},
    )
    st.plotly_chart(fig_pie, width="stretch", key="pie")

# ── 7b. Grouped bar: Avg WaitTime vs ServiceTime by Type ─────────────
with chart_right:
    fig_bar = go.Figure()
    fig_bar.add_trace(go.Bar(
        x=[_TYPE_LABELS.get(t, t) for t in type_avg["Type"]],
        y=type_avg["WaitTime"],
        name="Avg Wait Time",
        marker_color="#6b9dae",
        marker_line=dict(width=0),
        hovertemplate="<b>%{x}</b><br>Wait: %{y:.1f} ts<extra></extra>",
    ))
    fig_bar.add_trace(go.Bar(
        x=[_TYPE_LABELS.get(t, t) for t in type_avg["Type"]],
        y=type_avg["ServiceTime"],
        name="Avg Service Time",
        marker_color="#ae6b8f",
        marker_line=dict(width=0),
        hovertemplate="<b>%{x}</b><br>Service: %{y:.1f} ts<extra></extra>",
    ))
    

    fig_bar.update_layout(**_PLOTLY_LAYOUT)
    fig_bar.update_layout(
        title=dict(text="Avg Wait vs Service Time by Order Type", font=dict(size=14, color="#a0a0a0"), x=0.5),
        barmode="group",
        height=380,
        xaxis_title=None,
        yaxis_title="Time (ts)",
        xaxis=dict(tickangle=-25, tickfont=dict(size=10)),
    )
    st.plotly_chart(fig_bar, width="stretch", key="bar")

# ── 7c. Throughput line chart ─────────────────────────────────────────
st.markdown('<div class="section-header">Throughput — Cumulative Order Completion</div>', unsafe_allow_html=True)

if len(finished_df) > 0:
    fig_line = go.Figure()
    fig_line.add_trace(go.Scatter(
        x=finished_df["TF"],
        y=finished_df["CumulativeFinished"],
        mode="lines",
        line=dict(color="#6b9dae", width=2.5, shape="spline"),
        fill="tozeroy",
        fillcolor="rgba(107,157,174,0.08)",
        hovertemplate="Time: %{x} ts<br>Completed: %{y}<extra></extra>",
    ))
    
    # نفس الحل هنا عشان الخطأ ميتكررش
    fig_line.update_layout(**_PLOTLY_LAYOUT)
    fig_line.update_layout(
        title=dict(text="Cumulative Finished Orders Over Simulation Time", font=dict(size=14, color="#a0a0a0"), x=0.5),
        height=340,
        xaxis_title="Finish Time (TF)",
        yaxis_title="Cumulative Orders",
    )
    st.plotly_chart(fig_line, width="stretch", key="throughput")
else:
    st.info("No finished orders to display in the throughput chart.")

# ──────────────────────────────────────────────────────────────────────
# 8. DETAILED DATA TABLE
# ──────────────────────────────────────────────────────────────────────
st.markdown("---")
st.markdown('<div class="section-header">Raw Simulation Data</div>', unsafe_allow_html=True)

with st.expander("Show / Hide Data Table", expanded=False):
    display_df = df.copy()
    display_df["Type"] = display_df["Type"].map(lambda t: _TYPE_LABELS.get(t, t))
    st.dataframe(
        display_df.style.format(
            {c: "{:.0f}" for c in ["ID", "TQ", "TA", "TR", "TS", "TF", "WaitTime", "ServiceTime"]
             if c in display_df.columns}
        ),
        width="stretch",
        height=360,
    )

# ──────────────────────────────────────────────────────────────────────
# 9. FOOTER
# ──────────────────────────────────────────────────────────────────────
st.markdown(
    "<div style='text-align:center; padding:30px 0 12px 0; color:#3e4448; font-size:0.7rem;'>"
    " Analytics — Restaurant Simulation Dashboard&nbsp;&nbsp;·&nbsp;&nbsp;"
    "Built with Streamlit + Plotly"
    "</div>",
    unsafe_allow_html=True,
)