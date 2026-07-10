export default {
  async fetch(request, env, ctx) {
    const url = new URL(request.url);

    if (request.method === "OPTIONS") {
      return cors(new Response(null, { status: 204 }));
    }

    if (url.pathname === "/health") {
      return json({ ok: true, service: "todolist-telemetry" });
    }

    if (url.pathname === "/v1/report" && request.method === "POST") {
      return handleReport(request, env);
    }

    if (url.pathname === "/v1/stats" && request.method === "GET") {
      return handleStats(request, env);
    }

    return json({ error: "not found" }, 404);
  },
};

async function handleReport(request, env) {
  let body;
  try {
    body = await request.json();
  } catch {
    return json({ error: "invalid json" }, 400);
  }

  const installId = String(body.install_id || "").trim();
  const event = String(body.event || "").trim();
  if (!installId || !event) {
    return json({ error: "install_id and event required" }, 400);
  }

  const now = new Date().toISOString();
  const record = {
    install_id: installId,
    event,
    version: body.version || "",
    build: Number(body.build || 0),
    os: body.os || "",
    locale: body.locale || "",
    channel: body.channel || "stable",
    from_version: body.from_version || "",
    ts: body.ts || now,
    last_seen: now,
  };

  await env.TELEMETRY_KV.put(`install:${installId}`, JSON.stringify(record));

  if (env.TELEMETRY_DB) {
    await env.TELEMETRY_DB.prepare(
      `INSERT INTO events (install_id, event, version, build, os, locale, channel, from_version, ts)
       VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)`
    )
      .bind(
        record.install_id,
        record.event,
        record.version,
        record.build,
        record.os,
        record.locale,
        record.channel,
        record.from_version,
        record.ts
      )
      .run();
  }

  return json({ ok: true });
}

async function handleStats(request, env) {
  const token = new URL(request.url).searchParams.get("token");
  if (!token || token !== env.ADMIN_TOKEN) {
    return json({ error: "unauthorized" }, 401);
  }

  if (!env.TELEMETRY_DB) {
    return json({ error: "d1 not configured" }, 501);
  }

  const installs = await env.TELEMETRY_DB.prepare(
    `SELECT COUNT(DISTINCT install_id) AS cnt FROM events`
  ).first();

  const active7d = await env.TELEMETRY_DB.prepare(
    `SELECT COUNT(DISTINCT install_id) AS cnt
     FROM events
     WHERE ts >= datetime('now', '-7 days')`
  ).first();

  const versions = await env.TELEMETRY_DB.prepare(
    `SELECT version, COUNT(DISTINCT install_id) AS users
     FROM events
     WHERE event = 'app_start'
     GROUP BY version
     ORDER BY users DESC`
  ).all();

  return json({
    installs: installs?.cnt || 0,
    active_7d: active7d?.cnt || 0,
    versions: versions.results || [],
  });
}

function json(data, status = 200) {
  return cors(
    new Response(JSON.stringify(data), {
      status,
      headers: { "Content-Type": "application/json; charset=utf-8" },
    })
  );
}

function cors(response) {
  response.headers.set("Access-Control-Allow-Origin", "*");
  response.headers.set("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
  response.headers.set("Access-Control-Allow-Headers", "Content-Type");
  return response;
}
