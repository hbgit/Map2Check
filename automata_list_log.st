ldv_nonpositive@0@1059@ if(r&lt;0) return r;@[r &lt; 0]@1@1059@1060@1@0
ldv_nonpositive@1059@1059@ if(r&lt;0) return r;@NONE@0@NONE@NONE@1@0
ldv_nonpositive@1060@1060@ else return 0;@NONE@0@NONE@NONE@1@0
ldv_nonpositive@1061@1061@}@NONE@0@NONE@NONE@1@0
ldv_positive@1064@1064@ int r = __VERIFIER_nondet_int();@NONE@0@NONE@NONE@1@0
ldv_positive@1064@1064@ int r = __VERIFIER_nondet_int();@NONE@0@NONE@NONE@1@0
ldv_positive@0@1065@ if(r&gt;0) return r;@[r &gt; 0]@1@1065@1066@1@0
ldv_positive@1065@1065@ if(r&gt;0) return r;@NONE@0@NONE@NONE@1@0
ldv_positive@1066@1066@ else return 1;@NONE@0@NONE@NONE@1@0
ldv_positive@1067@1067@}@NONE@0@NONE@NONE@1@0
ldv_malloc@1072@1072@void *ldv_malloc(size_t size) {@NONE@0@NONE@NONE@1@0
ldv_malloc@1073@1073@ if(__VERIFIER_nondet_int()) {@NONE@0@NONE@NONE@1@0
ldv_malloc@0@1073@ if(__VERIFIER_nondet_int()) {@[ !0]@1@1074@1076@1@0
ldv_malloc@1074@1074@  return malloc(size);@NONE@0@NONE@NONE@1@0
ldv_malloc@1076@1076@  return 0;@NONE@0@NONE@NONE@1@0
ldv_malloc@1078@1078@};@NONE@0@NONE@NONE@1@0
ldv_zalloc@1080@1080@void *ldv_zalloc(size_t size) {@NONE@0@NONE@NONE@1@0
ldv_zalloc@0@1081@ return calloc(size, 1);@NONE@0@NONE@NONE@1@0
ldv_msg_alloc@1143@1143@ struct ldv_msg *msg;@NONE@0@NONE@NONE@1@0
ldv_msg_alloc@1144@1144@ msg = (struct ldv_msg*)ldv_malloc(sizeof(struct ldv_msg));@NONE@0@NONE@NONE@1@0
ldv_msg_alloc@0@1145@ if(msg) {@[ !msg]@1@1147@1149@1@0
ldv_msg_alloc@1147@1147@  LDV_INIT_LIST_HEAD(&amp;msg-&gt;list);@NONE@0@NONE@NONE@1@0
ldv_msg_alloc@1146@1147@  LDV_INIT_LIST_HEAD(&amp;msg-&gt;list);@NONE@0@NONE@NONE@1@0
ldv_msg_alloc@1149@1149@ return msg;@NONE@0@NONE@NONE@1@0
LDV_INIT_LIST_HEAD@1097@1097@static inline void LDV_INIT_LIST_HEAD(struct ldv_list_head *list)@NONE@0@NONE@NONE@1@0
LDV_INIT_LIST_HEAD@0@1101@}@NONE@0@NONE@NONE@1@0
ldv_msg_fill@1152@1152@int ldv_msg_fill(struct ldv_msg *msg, void *buf, int len) {@NONE@0@NONE@NONE@1@0
ldv_msg_fill@1152@1152@int ldv_msg_fill(struct ldv_msg *msg, void *buf, int len) {@NONE@0@NONE@NONE@1@0
ldv_msg_fill@1152@1152@int ldv_msg_fill(struct ldv_msg *msg, void *buf, int len) {@NONE@0@NONE@NONE@1@0
ldv_msg_fill@1153@1153@ void *data;@NONE@0@NONE@NONE@1@0
ldv_msg_fill@1154@1154@ data = ldv_malloc(len);@NONE@0@NONE@NONE@1@0
ldv_msg_fill@0@1155@ if(!data) return - -3;@[ !data]@1@1158@1155@1@0
ldv_msg_fill@1155@1155@ if(!data) return - -3;@NONE@0@NONE@NONE@1@0
ldv_msg_fill@1156@1156@ memcpy(data, buf, len);@NONE@0@NONE@NONE@1@0
ldv_msg_fill@1156@1158@ return 0;@NONE@0@NONE@NONE@1@0
ldv_msg_fill@1159@1159@}@NONE@0@NONE@NONE@1@0
ldv_msg_free@1161@1161@void ldv_msg_free(struct ldv_msg *msg) {@NONE@0@NONE@NONE@1@0
ldv_msg_free@0@1162@ if(msg) {@[ !]@1@1164@1166@1@0
ldv_msg_free@1163@1164@  free(msg);@NONE@0@NONE@NONE@1@0
ldv_msg_free@1166@1166@}@NONE@0@NONE@NONE@1@0
ldv_submit_msg@1168@1168@int ldv_submit_msg(struct ldv_msg *msg) {@NONE@0@NONE@NONE@1@0
ldv_submit_msg@1169@1169@  if(__VERIFIER_nondet_int()) {@NONE@0@NONE@NONE@1@0
ldv_submit_msg@0@1169@  if(__VERIFIER_nondet_int()) {@[ !0]@1@1171@1173@1@0
ldv_submit_msg@1170@1170@   ldv_list_add(&amp;msg-&gt;list, &amp;ldv_global_msg_list);@NONE@0@NONE@NONE@1@0
ldv_submit_msg@1170@1171@   return 0;@NONE@0@NONE@NONE@1@0
ldv_submit_msg@1173@1173@  return -1;@NONE@0@NONE@NONE@1@0
ldv_submit_msg@1174@1174@}@NONE@0@NONE@NONE@1@0
ldv_list_add@1119@1119@static inline void ldv_list_add(struct ldv_list_head *new, struct ldv_list_head *head)@NONE@0@NONE@NONE@1@0
ldv_list_add@1119@1119@static inline void ldv_list_add(struct ldv_list_head *new, struct ldv_list_head *head)@NONE@0@NONE@NONE@1@0
ldv_list_add@1121@1121@ __ldv_list_add(new, head, head-&gt;next);@NONE@0@NONE@NONE@1@0
ldv_list_add@0@1122@}@NONE@0@NONE@NONE@1@0
ldv_destroy_msgs@1177@1177@ struct ldv_msg *msg;@NONE@0@NONE@NONE@1@0
ldv_destroy_msgs@1178@1178@ struct ldv_msg *n;@NONE@0@NONE@NONE@1@0
ldv_destroy_msgs@1179@1179@ for (msg = ({ const typeof( ((typeof(*msg) *)0)-&gt;list ) *__mptr = ((&amp;ldv_global_msg_list)-&gt;next); (typeof(*msg) *)( (char *)__mptr - ((size_t) &amp;((typeof(*msg) *)0)-&gt;list) );}), n = ({ const typeof( ((typeof(*(msg)) *)0)-&gt;list ) *__mptr = ((msg)-&gt;list.next); (typeof(*(msg)) *)( (char *)__mptr - ((size_t) &amp;((typeof(*(msg)) *)0)-&gt;list) );}); &amp;msg-&gt;list != (&amp;ldv_global_msg_list); msg = n, n = ({ const typeof( ((typeof(*(n)) *)0)-&gt;list ) *__mptr = ((n)-&gt;list.next); (typeof(*(n)) *)( (char *)__mptr - ((size_t) &amp;((typeof(*(n)) *)0)-&gt;list) );})) {@NONE@0@NONE@NONE@1@0
ldv_destroy_msgs@1179@1179@ for (msg = ({ const typeof( ((typeof(*msg) *)0)-&gt;list ) *__mptr = ((&amp;ldv_global_msg_list)-&gt;next); (typeof(*msg) *)( (char *)__mptr - ((size_t) &amp;((typeof(*msg) *)0)-&gt;list) );}), n = ({ const typeof( ((typeof(*(msg)) *)0)-&gt;list ) *__mptr = ((msg)-&gt;list.next); (typeof(*(msg)) *)( (char *)__mptr - ((size_t) &amp;((typeof(*(msg)) *)0)-&gt;list) );}); &amp;msg-&gt;list != (&amp;ldv_global_msg_list); msg = n, n = ({ const typeof( ((typeof(*(n)) *)0)-&gt;list ) *__mptr = ((n)-&gt;list.next); (typeof(*(n)) *)( (char *)__mptr - ((size_t) &amp;((typeof(*(n)) *)0)-&gt;list) );})) {@NONE@0@NONE@NONE@1@0
ldv_destroy_msgs@0@1179@ for (msg = ({ const typeof( ((typeof(*msg) *)0)-&gt;list ) *__mptr = ((&amp;ldv_global_msg_list)-&gt;next); (typeof(*msg) *)( (char *)__mptr - ((size_t) &amp;((typeof(*msg) *)0)-&gt;list) );}), n = ({ const typeof( ((typeof(*(msg)) *)0)-&gt;list ) *__mptr = ((msg)-&gt;list.next); (typeof(*(msg)) *)( (char *)__mptr - ((size_t) &amp;((typeof(*(msg)) *)0)-&gt;list) );}); &amp;msg-&gt;list != (&amp;ldv_global_msg_list); msg = n, n = ({ const typeof( ((typeof(*(n)) *)0)-&gt;list ) *__mptr = ((n)-&gt;list.next); (typeof(*(n)) *)( (char *)__mptr - ((size_t) &amp;((typeof(*(n)) *)0)-&gt;list) );})) {@NONE@0@NONE@NONE@1@0
ldv_destroy_msgs@1179@1179@ for (msg = ({ const typeof( ((typeof(*msg) *)0)-&gt;list ) *__mptr = ((&amp;ldv_global_msg_list)-&gt;next); (typeof(*msg) *)( (char *)__mptr - ((size_t) &amp;((typeof(*msg) *)0)-&gt;list) );}), n = ({ const typeof( ((typeof(*(msg)) *)0)-&gt;list ) *__mptr = ((msg)-&gt;list.next); (typeof(*(msg)) *)( (char *)__mptr - ((size_t) &amp;((typeof(*(msg)) *)0)-&gt;list) );}); &amp;msg-&gt;list != (&amp;ldv_global_msg_list); msg = n, n = ({ const typeof( ((typeof(*(n)) *)0)-&gt;list ) *__mptr = ((n)-&gt;list.next); (typeof(*(n)) *)( (char *)__mptr - ((size_t) &amp;((typeof(*(n)) *)0)-&gt;list) );})) {@[ !]@1@1182@1184@1@0
ldv_destroy_msgs@1180@1180@  ldv_list_del(&amp;msg-&gt;list);@NONE@0@NONE@NONE@1@0
ldv_destroy_msgs@1182@1182@  ldv_msg_free(msg);@NONE@0@NONE@NONE@1@0
ldv_destroy_msgs@1180@1182@  ldv_msg_free(msg);@NONE@0@NONE@NONE@1@0
ldv_destroy_msgs@1179@1179@ for (msg = ({ const typeof( ((typeof(*msg) *)0)-&gt;list ) *__mptr = ((&amp;ldv_global_msg_list)-&gt;next); (typeof(*msg) *)( (char *)__mptr - ((size_t) &amp;((typeof(*msg) *)0)-&gt;list) );}), n = ({ const typeof( ((typeof(*(msg)) *)0)-&gt;list ) *__mptr = ((msg)-&gt;list.next); (typeof(*(msg)) *)( (char *)__mptr - ((size_t) &amp;((typeof(*(msg)) *)0)-&gt;list) );}); &amp;msg-&gt;list != (&amp;ldv_global_msg_list); msg = n, n = ({ const typeof( ((typeof(*(n)) *)0)-&gt;list ) *__mptr = ((n)-&gt;list.next); (typeof(*(n)) *)( (char *)__mptr - ((size_t) &amp;((typeof(*(n)) *)0)-&gt;list) );})) {@NONE@0@NONE@NONE@1@0
ldv_destroy_msgs@1179@1179@ for (msg = ({ const typeof( ((typeof(*msg) *)0)-&gt;list ) *__mptr = ((&amp;ldv_global_msg_list)-&gt;next); (typeof(*msg) *)( (char *)__mptr - ((size_t) &amp;((typeof(*msg) *)0)-&gt;list) );}), n = ({ const typeof( ((typeof(*(msg)) *)0)-&gt;list ) *__mptr = ((msg)-&gt;list.next); (typeof(*(msg)) *)( (char *)__mptr - ((size_t) &amp;((typeof(*(msg)) *)0)-&gt;list) );}); &amp;msg-&gt;list != (&amp;ldv_global_msg_list); msg = n, n = ({ const typeof( ((typeof(*(n)) *)0)-&gt;list ) *__mptr = ((n)-&gt;list.next); (typeof(*(n)) *)( (char *)__mptr - ((size_t) &amp;((typeof(*(n)) *)0)-&gt;list) );})) {@NONE@0@NONE@NONE@1@0
ldv_destroy_msgs@1184@1184@}@NONE@0@NONE@NONE@1@0
ldv_list_del@1129@1129@static inline void ldv_list_del(struct ldv_list_head *entry)@NONE@0@NONE@NONE@1@0
ldv_list_del@1131@1131@ __ldv_list_del(entry-&gt;prev, entry-&gt;next);@NONE@0@NONE@NONE@1@0
ldv_list_del@0@1134@}@NONE@0@NONE@NONE@1@0
ldv_kobject_del@1321@1321@void ldv_kobject_del(struct ldv_kobject *kobj)@NONE@0@NONE@NONE@1@0
ldv_kobject_del@0@1325@        if (!kobj)@[ !]@1@1334@1334@1@0
ldv_kobject_del@1334@1334@}@NONE@0@NONE@NONE@1@0
ldv_kobject_put@1359@1359@void ldv_kobject_put(struct ldv_kobject *kobj)@NONE@0@NONE@NONE@1@0
ldv_kobject_put@0@1361@        if (kobj) {@[ !]@1@1363@1365@1@0
ldv_kobject_put@1363@1363@                ldv_kref_put(&amp;kobj-&gt;kref, ldv_kobject_release);@NONE@0@NONE@NONE@1@0
ldv_kobject_put@1363@1363@                ldv_kref_put(&amp;kobj-&gt;kref, ldv_kobject_release);@NONE@0@NONE@NONE@1@0
ldv_kobject_put@1365@1365@}@NONE@0@NONE@NONE@1@0
ldv_kref_put@1311@1311@static inline int ldv_kref_put(struct ldv_kref *kref, void (*release)(struct ldv_kref *kref))@NONE@0@NONE@NONE@1@0
ldv_kref_put@1311@1311@static inline int ldv_kref_put(struct ldv_kref *kref, void (*release)(struct ldv_kref *kref))@NONE@0@NONE@NONE@1@0
ldv_kref_put@1313@1313@        return ldv_kref_sub(kref, 1, release);@NONE@0@NONE@NONE@1@0
ldv_kref_put@0@1313@        return ldv_kref_sub(kref, 1, release);@NONE@0@NONE@NONE@1@0
ldv_kobject_release@1348@1348@static void ldv_kobject_release(struct ldv_kref *kref) {@NONE@0@NONE@NONE@1@0
ldv_kobject_release@1349@1349@ struct ldv_kobject *kobj = ({ const typeof( ((struct ldv_kobject *)0)-&gt;kref ) *__mptr = (kref); (struct ldv_kobject *)( (char *)__mptr - ((size_t) &amp;((struct ldv_kobject *)0)-&gt;kref) );});@NONE@0@NONE@NONE@1@0
ldv_kobject_release@1349@1349@ struct ldv_kobject *kobj = ({ const typeof( ((struct ldv_kobject *)0)-&gt;kref ) *__mptr = (kref); (struct ldv_kobject *)( (char *)__mptr - ((size_t) &amp;((struct ldv_kobject *)0)-&gt;kref) );});@NONE@0@NONE@NONE@1@0
ldv_kobject_release@1350@1350@        ldv_kobject_cleanup(kobj);@NONE@0@NONE@NONE@1@0
ldv_kobject_release@0@1351@}@NONE@0@NONE@NONE@1@0
ldv_kobject_get@1371@1371@struct ldv_kobject *ldv_kobject_get(struct ldv_kobject *kobj)@NONE@0@NONE@NONE@1@0
ldv_kobject_get@0@1373@        if (kobj)@[ !]@1@1374@1375@1@0
ldv_kobject_get@1374@1374@                ldv_kref_get(&amp;kobj-&gt;kref);@NONE@0@NONE@NONE@1@0
ldv_kobject_get@1374@1374@                ldv_kref_get(&amp;kobj-&gt;kref);@NONE@0@NONE@NONE@1@0
ldv_kobject_get@1375@1375@        return kobj;@NONE@0@NONE@NONE@1@0
ldv_kref_get@1301@1301@static inline void ldv_kref_get(struct ldv_kref *kref)@NONE@0@NONE@NONE@1@0
ldv_kref_get@1308@1308@        ldv_atomic_add_return(1, (&amp;kref-&gt;refcount));@NONE@0@NONE@NONE@1@0
ldv_kref_get@0@1309@}@NONE@0@NONE@NONE@1@0
ldv_kobject_init@1390@1390@void ldv_kobject_init(struct ldv_kobject *kobj)@NONE@0@NONE@NONE@1@0
ldv_kobject_init@0@1394@        if (!kobj) {@[ !]@1@1398@1402@1@0
ldv_kobject_init@1398@1398@        ldv_kobject_init_internal(kobj);@NONE@0@NONE@NONE@1@0
ldv_kobject_init@1403@1403@}@NONE@0@NONE@NONE@1@0
ldv_kobject_init_internal@1378@1378@static void ldv_kobject_init_internal(struct ldv_kobject *kobj)@NONE@0@NONE@NONE@1@0
ldv_kobject_init_internal@0@1380@        if (!kobj)@[ !]@1@1383@1388@1@0
ldv_kobject_init_internal@1382@1383@        LDV_INIT_LIST_HEAD(&amp;kobj-&gt;entry);@NONE@0@NONE@NONE@1@0
ldv_kobject_init_internal@1388@1388@}@NONE@0@NONE@NONE@1@0
ldv_kobject_create@1406@1406@        struct ldv_kobject *kobj;@NONE@0@NONE@NONE@1@0
ldv_kobject_create@1408@1408@        kobj = ldv_malloc(sizeof(*kobj));@NONE@0@NONE@NONE@1@0
ldv_kobject_create@0@1409@        if (!kobj)@[ !kobj]@1@1414@1410@1@0
ldv_kobject_create@1410@1410@                return 0;@NONE@0@NONE@NONE@1@0
ldv_kobject_create@1411@1411@ memset(kobj, 0, sizeof(*kobj));@NONE@0@NONE@NONE@1@0
ldv_kobject_create@1413@1413@        ldv_kobject_init(kobj);@NONE@0@NONE@NONE@1@0
ldv_kobject_create@1411@1414@        return kobj;@NONE@0@NONE@NONE@1@0
ldv_kobject_create@1415@1415@}@NONE@0@NONE@NONE@1@0
f@1426@1426@ return __VERIFIER_nondet_int();@NONE@0@NONE@NONE@1@0
f@1426@1426@ return __VERIFIER_nondet_int();@NONE@0@NONE@NONE@1@0
g@1430@1430@ return __VERIFIER_nondet_int();@NONE@0@NONE@NONE@1@0
g@1430@1430@ return __VERIFIER_nondet_int();@NONE@0@NONE@NONE@1@0
entry_point@1436@1436@ struct ldv_kobject *kobj;@NONE@0@NONE@NONE@1@0
entry_point@1437@1437@ kobj = ldv_kobject_create();@NONE@0@NONE@NONE@1@0
entry_point@1438@1438@ ldv_kobject_put(kobj);@NONE@0@NONE@NONE@1@0
entry_point@1440@1440@ ldv_kobject_put(kobj);@NONE@0@NONE@NONE@1@0
entry_point@0@1441@}@NONE@0@NONE@NONE@1@0
main@1444@1444@     entry_point();@NONE@0@NONE@NONE@1@0
main@1444@1444@     entry_point();@NONE@0@NONE@NONE@0@0
__ldv_list_add@1103@1103@static inline void __ldv_list_add(struct ldv_list_head *new,@NONE@0@NONE@NONE@1@0
__ldv_list_add@1104@1104@                            struct ldv_list_head *prev,@NONE@0@NONE@NONE@1@0
__ldv_list_add@1105@1105@                            struct ldv_list_head *next)@NONE@0@NONE@NONE@1@0
__ldv_list_add@0@1111@}@NONE@0@NONE@NONE@1@0
__ldv_list_del@1113@1113@static inline void __ldv_list_del(struct ldv_list_head * prev, struct ldv_list_head * next)@NONE@0@NONE@NONE@1@0
__ldv_list_del@1113@1113@static inline void __ldv_list_del(struct ldv_list_head * prev, struct ldv_list_head * next)@NONE@0@NONE@NONE@1@0
__ldv_list_del@0@1117@}@NONE@0@NONE@NONE@1@0
ldv_kref_sub@1276@1276@static inline int ldv_kref_sub(struct ldv_kref *kref, unsigned int count,@NONE@0@NONE@NONE@1@0
ldv_kref_sub@1276@1276@static inline int ldv_kref_sub(struct ldv_kref *kref, unsigned int count,@NONE@0@NONE@NONE@1@0
ldv_kref_sub@1277@1277@            void (*release)(struct ldv_kref *kref))@NONE@0@NONE@NONE@1@0
ldv_kref_sub@1281@1281@        if ((ldv_atomic_sub_return(((int) count), (&amp;kref-&gt;refcount)) == 0)) {@NONE@0@NONE@NONE@1@0
ldv_kref_sub@0@1281@        if ((ldv_atomic_sub_return(((int) count), (&amp;kref-&gt;refcount)) == 0)) {@[ == 0]@1@1283@1285@1@0
