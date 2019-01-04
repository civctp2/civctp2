project('anet', 'c', version : '2.11')

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
	'src/tca/crypttab.c'
	]

c_args = [ '-Ddp_ANET2', '-DPACK=__attribute__ ((packed))' ]

if host_machine.system() == 'windows'
	sources += [
		'src/win/dp/dllmain.c',
		'src/win/dp/dpdllwin.c',
		'src/win/dp/enumapp.c',
		'src/win/dp/launchap.c',
		'src/linux/uudp/uudps.c'
	]
endif
if host_machine.system() == 'linux'
	sources += [
		'src/linux/dp/dpdll.c',
		'src/linux/dp/dpmem.c',
		'src/linux/dp/enumapp.c',
		'src/linux/dp/launchap.c',
		'src/linux/uudp/uudps.c'
	]
	c_args += [ '-DUNIX', '-Dcdecl=', '-Dstrnicmp=strncasecmp', '-Dstricmp=strcasecmp', '-DCOMM_INST' ]
endif

anet = shared_library(
	meson.project_name(), sources,
	version : meson.project_version(),
	include_directories : include_directories('h', 'src', 'src/aeh', 'demo/gtest', 'demo/utils'),
	c_args : c_args)

libanet = declare_dependency(link_with : anet, include_directories : include_directories('h'))

