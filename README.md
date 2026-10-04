# Lunar

Lunar is a tiny embedded key-value database.

Just an architectural scaffold for now.

The C engine will append records to one file and keep an in-memory hash index of
key to latest record offset. Opening the file rebuilds the index. Go bindings
and tooling will come after the core engine works.

