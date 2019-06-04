# meson.build is a fast and furious build system
# minimum meson version required for best results: 0.32.0

project('qgit', 'cpp', default_options : ['cpp_std=c++11', 'warning_level=3'])

qt5 = import('qt5')
qt5_dep = dependency('qt5', modules : ['Core', 'Gui', 'Widgets'])

incdir = include_directories('src')

qgit_headers = [
    'src/annotate.h',
    'src/cache.h',
    'src/commitimpl.h',
    'src/common.h',
    'src/config.h',
    'src/consoleimpl.h',
    'src/customactionimpl.h',
    'src/dataloader.h',
    'src/domain.h',
    'src/exceptionmanager.h',
    'src/filecontent.h',
    'src/filehistory.h',
    'src/filelist.h',
    'src/fileview.h',
    'src/git.h',
    'src/help.h',
    'src/inputdialog.h',
    'src/lanes.h',
    'src/listview.h',
    'src/mainimpl.h',
    'src/myprocess.h',
    'src/patchcontent.h',
    'src/patchview.h',
    'src/rangeselectimpl.h',
    'src/revdesc.h',
    'src/revision.h',
    'src/revsview.h',
    'src/settingsimpl.h',
    'src/smartbrowse.h',
    'src/treeview.h'
]

qgit_sources = [
    'src/annotate.cpp',
    'src/cache.cpp',
    'src/commitimpl.cpp',
    'src/common.cpp',
    'src/consoleimpl.cpp',
    'src/customactionimpl.cpp',
    'src/dataloader.cpp',
    'src/domain.cpp',
    'src/exceptionmanager.cpp',
    'src/filecontent.cpp',
    'src/filehistory.cpp',
    'src/filelist.cpp',
    'src/fileview.cpp',
    'src/git.cpp',
    'src/inputdialog.cpp',
    'src/lanes.cpp',
    'src/listview.cpp',
    'src/mainimpl.cpp',
    'src/myprocess.cpp',
    'src/namespace_def.cpp',
    'src/patchcontent.cpp',
    'src/patchview.cpp',
    'src/qgit.cpp',
    'src/rangeselectimpl.cpp',
    'src/revdesc.cpp',
    'src/revision.cpp',
    'src/revsview.cpp',
    'src/settingsimpl.cpp',
    'src/smartbrowse.cpp',
    'src/treeview.cpp'
]

qgit_resources = [
    'src/icons.qrc'
]

qgit_forms = [
    'src/commit.ui',
    'src/console.ui',
    'src/customaction.ui',
    'src/fileview.ui',
    'src/help.ui',
    'src/mainview.ui',
    'src/patchview.ui',
    'src/rangeselect.ui',
    'src/revsview.ui',
    'src/settings.ui'
]

prep = qt5.preprocess(
                    moc_headers : qgit_headers,
                    ui_files : qgit_forms,
                    qresources : qgit_resources,
                    include_directories : incdir,
                    dependencies: qt5_dep
                    )

q5exe = executable('qgit',
                    sources : [qgit_sources, prep],
                    dependencies : [qt5_dep],
                    include_directories : incdir,
                    install : true
                    )

install_data('COPYING.rtf', install_dir : 'share/doc/qgit')
install_data('README', install_dir : 'share/doc/qgit')
install_data('exception_manager.txt', install_dir : 'share/doc/qgit')

install_data('src/resources/qgit.png', install_dir : 'share/pixmaps')
install_data('qgit.desktop', install_dir : 'share/applications')
install_data('qgit.appdata.xml', install_dir : 'share/appdata')

# translations = qt5.compile_translations(ts_files : 'qgit_ru.ts', build_by_default : true)
