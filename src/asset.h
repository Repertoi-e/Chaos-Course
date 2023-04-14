#pragma once

#include <lstd/array.h>
#include <lstd/common.h>
#include <lstd/hash_table.h>
#include <lstd/string.h>

struct asset {
  enum asset_type { None = 0, Shader = 1 };
  asset_type Type;
};

struct asset_key {
  string Path;

  bool operator==(asset_key no_copy other) const {
    return strings_match(Path, other.Path);
  }
};

struct asset_data {
  asset *Asset = null;

  string FilePath;
  time_t FileLastModifiedTime = 0;
};

struct asset_store {
  hash_table<asset_key, asset_data> AssetFromKey;
};

inline asset_store g_AssetStore;

asset *get_asset_from_key(asset_key key) {}
