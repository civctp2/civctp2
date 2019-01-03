project('anet', 'c', version : '0.11')

sources = [
	'demo/utils/dpcrc32.c',
	'demo/utils/logpkt.c',
	'demo/utils/mywcs.c',
	'src/3rdparty/md5/md5c.c',
	'src/3rdparty/d3des/d3des.c',
	'src/aeh/aeh.c',
	'src/aeh/aehmap.c',
	'src/aeh/aehlog.c',
	'src/dp/dictset.c',
	'src/dp/dp2.c',
	'src/dp/dynatab.c',
	'src/dp/assoctab.c',
	'src/dp/dprint.c',
	'src/dp/q.c',
	'src/dp/ini.c',
	'src/dp/dpuser.c',
	'src/dp/dpio.c',
	'src/dp/hkeytab.c',
	'src/dp/dptab.c',
	'src/dp/delay.c',
	'src/dp/enumsrv2.c',
	'src/dp/dpexcept.c',
	'src/dp/dpprivy.c',
	'src/dp/dpscore.c',
	'src/dp/deltas.c',
	'src/dp/dpshutdn.c',
	'src/dp/launchparams.c',
	'src/score/scorerep.c',
	'src/tca/tserv.c',
	'src/tca/tserva.c',
	'src/tca/tca.c',
	'src/tca/tcapw.c',
	'src/tca/crypttab.c',
	'src/linux/dp/dpdll.c',
	'src/linux/dp/dpmem.c',
	'src/linux/dp/enumapp.c',
	'src/linux/dp/launchap.c',
	'src/linux/uudp/uudps.c'
]

anet = shared_library(
  'anet', sources,
  version : '0.11',
  include_directories : include_directories('h', 'src', 'src/aeh', 'src/linux/dp', 'demo/gtest', 'demo/utils'),
  c_args : [ '-Ddp_ANET2', '-DUNIX', '-Dcdecl=', '-DPACK=__attribute__ ((packed))', '-Dstrnicmp=strncasecmp', '-Dstricmp=strcasecmp', '-DCOMM_INST'],
  install : true)

libanet = declare_dependency(link_with : anet, include_directories : include_directories('h'))

