-- ToDoList SQLite schema v1
PRAGMA foreign_keys = ON;

CREATE TABLE IF NOT EXISTS tasks (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    title           TEXT NOT NULL,
    due_at          TEXT,
    quadrant        INTEGER NOT NULL DEFAULT 2,
    completed       INTEGER NOT NULL DEFAULT 0,
    rule_score      REAL NOT NULL DEFAULT 0,
    ai_rank         INTEGER NOT NULL DEFAULT 0,
    ai_reason       TEXT,
    created_at      TEXT NOT NULL,
    updated_at      TEXT NOT NULL
);

CREATE INDEX IF NOT EXISTS idx_tasks_due ON tasks(due_at);
CREATE INDEX IF NOT EXISTS idx_tasks_quadrant ON tasks(quadrant);
