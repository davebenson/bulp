
/* Files involved:
    .checkpoint => (hardlink) .journal
    .#.sorted.#.index
    .#.sorted.#.keys   == raw keys
    .#.sorted.stats
    .journal  == base-info and raw records
 */


BulpIndexer *
bulp_indexer_new (const char *filename_prefix,
                  BulpIndexerOptions *options)
{
  ...
}
