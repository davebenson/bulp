
BulpReader *bulp_merge_two_readers           (BulpComparator *key_comparator,
                                              BulpMerger *value_merger,
                                              BulpReader *reader_a,
                                              BulpReader *reader_b);
                         
void        bulp_merge_two_readers_to_writer (BulpComparator *key_comparator,
                                              BulpMerger *value_merger,
                                              BulpReader *reader_a,
                                              BulpReader *reader_b,
                                              BulpWriter *dest);

