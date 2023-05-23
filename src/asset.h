#pragma once

#include <lstd/array.h>
#include <lstd/common.h>
#include <lstd/hash_table.h>
#include <lstd/path.h>
#include <lstd/string.h>

struct asset {
  enum asset_type { Stub = 0, Shader = 1 };
  asset_type Type = Stub;
};

struct asset_key {
  string Path;

  asset_key() {}
  asset_key(string path) : Path(path) {}

  bool operator==(asset_key no_copy other) const {
    return strings_match(Path, other.Path);
  }
};

inline u64 get_hash(asset_key key) { return get_hash(key.Path); }

struct asset_data {
  asset *Asset = null;

  string FilePath;
  time_t FileLastModifiedTime = 0;

  s32 ReloadTimeOut = 0;
};

struct asset_store {
  hash_table<asset_key, asset_data> AssetFromKeyCached;
};

inline asset_store g_AssetStore;

struct load_result {
  asset *Asset = null;
  bool KeepOldIfReload = false;
};

using asset_loader = load_result (*)(string filePath);
using asset_unloader = void (*)(asset *asst);

// The loader gets called only if this is the first time we load the asset or
// the file on disc has changed since the last time it was loaded.
// The loader must always return a valid asset (a stub one if loading fails).
inline asset *get_asset_from_key_maybe_cached(asset_key key,
                                              asset_loader loader,
                                              asset_unloader unloader) {
  bool load = true;

  auto [_, value] = search(g_AssetStore.AssetFromKeyCached, key);
  if (value) {
    if (value->FilePath.Count) {
      if (strings_match(value->FilePath, key.Path)) {
        if (path_exists(key.Path) && path_last_modification_time(key.Path) ==
                                         value->FileLastModifiedTime) {
          load = false;
        } else {
          // Avoid trying to reload on every frame
          if (value->ReloadTimeOut != 0) {
            load = false;
            value->ReloadTimeOut--;
          }
        }
      }
    }
  }

  if (!load)
    return value->Asset; // Cached

  auto [loaded, keepOldIfReload] = loader(key.Path);
  assert(loaded);

  asset *result;

  if (!value || !keepOldIfReload) {
    if (!value) {
      auto [_, addedValue] = add(g_AssetStore.AssetFromKeyCached, key, {});
      value = addedValue;
      value->FilePath = key.Path;
    } else {
      unloader(value->Asset);
    }
    value->Asset = loaded;
    result = loaded;
  } else {
    result = value->Asset;
  }

  value->FilePath = key.Path;
  value->FileLastModifiedTime = path_last_modification_time(key.Path);
  value->ReloadTimeOut = 5;
  return result;
}
