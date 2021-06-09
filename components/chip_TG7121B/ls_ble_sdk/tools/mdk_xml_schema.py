# .\mdk.py
# -*- coding: utf-8 -*-
# PyXB bindings for NM:e92452c8d3e28a9e27abfc9994d2007779e7f4c9
# Generated 2020-02-18 21:18:26.126252 by PyXB version 1.2.6 using Python 3.8.1.final.0
# Namespace AbsentNamespace0

from __future__ import unicode_literals
import pyxb
import pyxb.binding
import pyxb.binding.saxer
import io
import pyxb.utils.utility
import pyxb.utils.domutils
import sys
import pyxb.utils.six as _six
# Unique identifier for bindings created at the same time
_GenerationUID = pyxb.utils.utility.UniqueIdentifier('urn:uuid:255a6f17-5251-11ea-acd3-0871908f3e6a')

# Version of PyXB used to generate the bindings
_PyXBVersion = '1.2.6'
# Generated bindings are not compatible across PyXB versions
if pyxb.__version__ != _PyXBVersion:
    raise pyxb.PyXBVersionError(_PyXBVersion)

# A holder for module-level binding classes so we can access them from
# inside class definitions where property names may conflict.
_module_typeBindings = pyxb.utils.utility.Object()

# Import bindings for namespaces imported into schema
import pyxb.binding.datatypes

# NOTE: All namespace declarations are reserved within the binding
Namespace = pyxb.namespace.CreateAbsentNamespace()
Namespace.configureCategories(['typeBinding', 'elementBinding'])

def CreateFromDocument (xml_text, default_namespace=None, location_base=None):
    """Parse the given XML and use the document element to create a
    Python instance.

    @param xml_text An XML document.  This should be data (Python 2
    str or Python 3 bytes), or a text (Python 2 unicode or Python 3
    str) in the L{pyxb._InputEncoding} encoding.

    @keyword default_namespace The L{pyxb.Namespace} instance to use as the
    default namespace where there is no default namespace in scope.
    If unspecified or C{None}, the namespace of the module containing
    this function will be used.

    @keyword location_base: An object to be recorded as the base of all
    L{pyxb.utils.utility.Location} instances associated with events and
    objects handled by the parser.  You might pass the URI from which
    the document was obtained.
    """

    if pyxb.XMLStyle_saxer != pyxb._XMLStyle:
        dom = pyxb.utils.domutils.StringToDOM(xml_text)
        return CreateFromDOM(dom.documentElement, default_namespace=default_namespace)
    if default_namespace is None:
        default_namespace = Namespace.fallbackNamespace()
    saxer = pyxb.binding.saxer.make_parser(fallback_namespace=default_namespace, location_base=location_base)
    handler = saxer.getContentHandler()
    xmld = xml_text
    if isinstance(xmld, _six.text_type):
        xmld = xmld.encode(pyxb._InputEncoding)
    saxer.parse(io.BytesIO(xmld))
    instance = handler.rootObject()
    return instance

def CreateFromDOM (node, default_namespace=None):
    """Create a Python instance from the given DOM node.
    The node tag must correspond to an element declaration in this module.

    @deprecated: Forcing use of DOM interface is unnecessary; use L{CreateFromDocument}."""
    if default_namespace is None:
        default_namespace = Namespace.fallbackNamespace()
    return pyxb.binding.basis.element.AnyCreateFromDOM(node, default_namespace)


# Atomic simple type: switcher
class switcher (pyxb.binding.datatypes.integer):

    """An atomic simple type."""

    _ExpandedName = pyxb.namespace.ExpandedName(Namespace, 'switcher')
    _XSDLocation = pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 4, 2)
    _Documentation = None
switcher._CF_minInclusive = pyxb.binding.facets.CF_minInclusive(value_datatype=switcher, value=pyxb.binding.datatypes.integer(0))
switcher._CF_maxInclusive = pyxb.binding.facets.CF_maxInclusive(value_datatype=switcher, value=pyxb.binding.datatypes.integer(1))
switcher._InitializeFacetMap(switcher._CF_minInclusive,
   switcher._CF_maxInclusive)
Namespace.addCategoryObject('typeBinding', 'switcher', switcher)
_module_typeBindings.switcher = switcher

# Complex type UserAction with content type SIMPLE
class UserAction (pyxb.binding.basis.complexTypeDefinition):
    """Complex type UserAction with content type SIMPLE"""
    _TypeDefinition = pyxb.binding.datatypes.string
    _ContentTypeTag = pyxb.binding.basis.complexTypeDefinition._CT_SIMPLE
    _Abstract = False
    _ExpandedName = pyxb.namespace.ExpandedName(Namespace, 'UserAction')
    _XSDLocation = pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 12, 2)
    _ElementMap = {}
    _AttributeMap = {}
    # Base type is pyxb.binding.datatypes.string
    
    # Attribute run uses Python identifier run
    __run = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'run'), 'run', '__AbsentNamespace0_UserAction_run', pyxb.binding.datatypes.boolean, unicode_default='1')
    __run._DeclarationLocation = pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 16, 8)
    __run._UseLocation = pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 16, 8)
    
    run = property(__run.value, __run.set, None, None)

    
    # Attribute Dos16Mode uses Python identifier Dos16Mode
    __Dos16Mode = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'Dos16Mode'), 'Dos16Mode', '__AbsentNamespace0_UserAction_Dos16Mode', pyxb.binding.datatypes.boolean, unicode_default='0')
    __Dos16Mode._DeclarationLocation = pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 18, 8)
    __Dos16Mode._UseLocation = pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 18, 8)
    
    Dos16Mode = property(__Dos16Mode.value, __Dos16Mode.set, None, None)

    
    # Attribute StopOnExitCode uses Python identifier StopOnExitCode
    __StopOnExitCode = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'StopOnExitCode'), 'StopOnExitCode', '__AbsentNamespace0_UserAction_StopOnExitCode', pyxb.binding.datatypes.decimal)
    __StopOnExitCode._DeclarationLocation = pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 20, 8)
    __StopOnExitCode._UseLocation = pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 20, 8)
    
    StopOnExitCode = property(__StopOnExitCode.value, __StopOnExitCode.set, None, None)

    _ElementMap.update({
        
    })
    _AttributeMap.update({
        __run.name() : __run,
        __Dos16Mode.name() : __Dos16Mode,
        __StopOnExitCode.name() : __StopOnExitCode
    })
_module_typeBindings.UserAction = UserAction
Namespace.addCategoryObject('typeBinding', 'UserAction', UserAction)


# Complex type [anonymous] with content type ELEMENT_ONLY
class CTD_ANON (pyxb.binding.basis.complexTypeDefinition):
    """Complex type [anonymous] with content type ELEMENT_ONLY"""
    _TypeDefinition = None
    _ContentTypeTag = pyxb.binding.basis.complexTypeDefinition._CT_ELEMENT_ONLY
    _Abstract = False
    _ExpandedName = None
    _XSDLocation = pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 26, 4)
    _ElementMap = {}
    _AttributeMap = {}
    # Base type is pyxb.binding.datatypes.anyType
    
    # Element ToolsetName uses Python identifier ToolsetName
    __ToolsetName = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'ToolsetName'), 'ToolsetName', '__AbsentNamespace0_CTD_ANON_ToolsetName', False, pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 29, 8), )

    
    ToolsetName = property(__ToolsetName.value, __ToolsetName.set, None, None)

    
    # Element Device uses Python identifier Device
    __Device = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'Device'), 'Device', '__AbsentNamespace0_CTD_ANON_Device', False, pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 31, 8), )

    
    Device = property(__Device.value, __Device.set, None, None)

    
    # Element Output uses Python identifier Output
    __Output = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'Output'), 'Output', '__AbsentNamespace0_CTD_ANON_Output', False, pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 33, 8), )

    
    Output = property(__Output.value, __Output.set, None, None)

    
    # Element User uses Python identifier User
    __User = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'User'), 'User', '__AbsentNamespace0_CTD_ANON_User', False, pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 44, 8), )

    
    User = property(__User.value, __User.set, None, None)

    
    # Element IncludePath uses Python identifier IncludePath
    __IncludePath = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'IncludePath'), 'IncludePath', '__AbsentNamespace0_CTD_ANON_IncludePath', False, pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 59, 8), )

    
    IncludePath = property(__IncludePath.value, __IncludePath.set, None, None)

    
    # Element ScatterFile uses Python identifier ScatterFile
    __ScatterFile = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'ScatterFile'), 'ScatterFile', '__AbsentNamespace0_CTD_ANON_ScatterFile', False, pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 61, 8), )

    
    ScatterFile = property(__ScatterFile.value, __ScatterFile.set, None, None)

    
    # Element SFDFile uses Python identifier SFDFile
    __SFDFile = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'SFDFile'), 'SFDFile', '__AbsentNamespace0_CTD_ANON_SFDFile', False, pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 63, 8), )

    
    SFDFile = property(__SFDFile.value, __SFDFile.set, None, None)

    
    # Element MiscControls uses Python identifier MiscControls
    __MiscControls = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'MiscControls'), 'MiscControls', '__AbsentNamespace0_CTD_ANON_MiscControls', False, pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 65, 8), )

    
    MiscControls = property(__MiscControls.value, __MiscControls.set, None, None)

    
    # Element CDefines uses Python identifier CDefines
    __CDefines = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'CDefines'), 'CDefines', '__AbsentNamespace0_CTD_ANON_CDefines', False, pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 67, 8), )

    
    CDefines = property(__CDefines.value, __CDefines.set, None, None)

    
    # Element AsmDefines uses Python identifier AsmDefines
    __AsmDefines = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'AsmDefines'), 'AsmDefines', '__AbsentNamespace0_CTD_ANON_AsmDefines', False, pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 69, 8), )

    
    AsmDefines = property(__AsmDefines.value, __AsmDefines.set, None, None)

    
    # Element COptions uses Python identifier COptions
    __COptions = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'COptions'), 'COptions', '__AbsentNamespace0_CTD_ANON_COptions', False, pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 71, 8), )

    
    COptions = property(__COptions.value, __COptions.set, None, None)

    
    # Element AsmOptions uses Python identifier AsmOptions
    __AsmOptions = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'AsmOptions'), 'AsmOptions', '__AbsentNamespace0_CTD_ANON_AsmOptions', False, pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 73, 8), )

    
    AsmOptions = property(__AsmOptions.value, __AsmOptions.set, None, None)

    
    # Element LinkOptions uses Python identifier LinkOptions
    __LinkOptions = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'LinkOptions'), 'LinkOptions', '__AbsentNamespace0_CTD_ANON_LinkOptions', False, pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 75, 8), )

    
    LinkOptions = property(__LinkOptions.value, __LinkOptions.set, None, None)

    
    # Element useUlib uses Python identifier useUlib
    __useUlib = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'useUlib'), 'useUlib', '__AbsentNamespace0_CTD_ANON_useUlib', False, pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 77, 8), )

    
    useUlib = property(__useUlib.value, __useUlib.set, None, None)

    
    # Element Simulator uses Python identifier Simulator
    __Simulator = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'Simulator'), 'Simulator', '__AbsentNamespace0_CTD_ANON_Simulator', False, pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 80, 8), )

    
    Simulator = property(__Simulator.value, __Simulator.set, None, None)

    
    # Element Target uses Python identifier Target
    __Target = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'Target'), 'Target', '__AbsentNamespace0_CTD_ANON_Target', False, pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 90, 8), )

    
    Target = property(__Target.value, __Target.set, None, None)

    
    # Element Groups uses Python identifier Groups
    __Groups = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'Groups'), 'Groups', '__AbsentNamespace0_CTD_ANON_Groups', False, pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 104, 8), )

    
    Groups = property(__Groups.value, __Groups.set, None, None)

    _ElementMap.update({
        __ToolsetName.name() : __ToolsetName,
        __Device.name() : __Device,
        __Output.name() : __Output,
        __User.name() : __User,
        __IncludePath.name() : __IncludePath,
        __ScatterFile.name() : __ScatterFile,
        __SFDFile.name() : __SFDFile,
        __MiscControls.name() : __MiscControls,
        __CDefines.name() : __CDefines,
        __AsmDefines.name() : __AsmDefines,
        __COptions.name() : __COptions,
        __AsmOptions.name() : __AsmOptions,
        __LinkOptions.name() : __LinkOptions,
        __useUlib.name() : __useUlib,
        __Simulator.name() : __Simulator,
        __Target.name() : __Target,
        __Groups.name() : __Groups
    })
    _AttributeMap.update({
        
    })
_module_typeBindings.CTD_ANON = CTD_ANON


# Complex type [anonymous] with content type EMPTY
class CTD_ANON_ (pyxb.binding.basis.complexTypeDefinition):
    """Complex type [anonymous] with content type EMPTY"""
    _TypeDefinition = None
    _ContentTypeTag = pyxb.binding.basis.complexTypeDefinition._CT_EMPTY
    _Abstract = False
    _ExpandedName = None
    _XSDLocation = pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 34, 10)
    _ElementMap = {}
    _AttributeMap = {}
    # Base type is pyxb.binding.datatypes.anyType
    
    # Attribute name uses Python identifier name
    __name = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'name'), 'name', '__AbsentNamespace0_CTD_ANON__name', pyxb.binding.datatypes.string)
    __name._DeclarationLocation = pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 36, 12)
    __name._UseLocation = pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 36, 12)
    
    name = property(__name.value, __name.set, None, None)

    
    # Attribute path uses Python identifier path
    __path = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'path'), 'path', '__AbsentNamespace0_CTD_ANON__path', pyxb.binding.datatypes.string)
    __path._DeclarationLocation = pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 38, 12)
    __path._UseLocation = pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 38, 12)
    
    path = property(__path.value, __path.set, None, None)

    
    # Attribute CreateLib uses Python identifier CreateLib
    __CreateLib = pyxb.binding.content.AttributeUse(pyxb.namespace.ExpandedName(None, 'CreateLib'), 'CreateLib', '__AbsentNamespace0_CTD_ANON__CreateLib', pyxb.binding.datatypes.boolean, unicode_default='0')
    __CreateLib._DeclarationLocation = pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 40, 12)
    __CreateLib._UseLocation = pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 40, 12)
    
    CreateLib = property(__CreateLib.value, __CreateLib.set, None, None)

    _ElementMap.update({
        
    })
    _AttributeMap.update({
        __name.name() : __name,
        __path.name() : __path,
        __CreateLib.name() : __CreateLib
    })
_module_typeBindings.CTD_ANON_ = CTD_ANON_


# Complex type [anonymous] with content type ELEMENT_ONLY
class CTD_ANON_2 (pyxb.binding.basis.complexTypeDefinition):
    """Complex type [anonymous] with content type ELEMENT_ONLY"""
    _TypeDefinition = None
    _ContentTypeTag = pyxb.binding.basis.complexTypeDefinition._CT_ELEMENT_ONLY
    _Abstract = False
    _ExpandedName = None
    _XSDLocation = pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 45, 10)
    _ElementMap = {}
    _AttributeMap = {}
    # Base type is pyxb.binding.datatypes.anyType
    
    # Element BeforeCompile1 uses Python identifier BeforeCompile1
    __BeforeCompile1 = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'BeforeCompile1'), 'BeforeCompile1', '__AbsentNamespace0_CTD_ANON_2_BeforeCompile1', False, pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 48, 14), )

    
    BeforeCompile1 = property(__BeforeCompile1.value, __BeforeCompile1.set, None, None)

    
    # Element BeforeCompile2 uses Python identifier BeforeCompile2
    __BeforeCompile2 = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'BeforeCompile2'), 'BeforeCompile2', '__AbsentNamespace0_CTD_ANON_2_BeforeCompile2', False, pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 49, 14), )

    
    BeforeCompile2 = property(__BeforeCompile2.value, __BeforeCompile2.set, None, None)

    
    # Element BeforeBuild1 uses Python identifier BeforeBuild1
    __BeforeBuild1 = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'BeforeBuild1'), 'BeforeBuild1', '__AbsentNamespace0_CTD_ANON_2_BeforeBuild1', False, pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 50, 14), )

    
    BeforeBuild1 = property(__BeforeBuild1.value, __BeforeBuild1.set, None, None)

    
    # Element BeforeBuild2 uses Python identifier BeforeBuild2
    __BeforeBuild2 = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'BeforeBuild2'), 'BeforeBuild2', '__AbsentNamespace0_CTD_ANON_2_BeforeBuild2', False, pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 51, 14), )

    
    BeforeBuild2 = property(__BeforeBuild2.value, __BeforeBuild2.set, None, None)

    
    # Element AfterBuild1 uses Python identifier AfterBuild1
    __AfterBuild1 = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'AfterBuild1'), 'AfterBuild1', '__AbsentNamespace0_CTD_ANON_2_AfterBuild1', False, pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 52, 14), )

    
    AfterBuild1 = property(__AfterBuild1.value, __AfterBuild1.set, None, None)

    
    # Element AfterBuild2 uses Python identifier AfterBuild2
    __AfterBuild2 = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'AfterBuild2'), 'AfterBuild2', '__AbsentNamespace0_CTD_ANON_2_AfterBuild2', False, pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 53, 14), )

    
    AfterBuild2 = property(__AfterBuild2.value, __AfterBuild2.set, None, None)

    _ElementMap.update({
        __BeforeCompile1.name() : __BeforeCompile1,
        __BeforeCompile2.name() : __BeforeCompile2,
        __BeforeBuild1.name() : __BeforeBuild1,
        __BeforeBuild2.name() : __BeforeBuild2,
        __AfterBuild1.name() : __AfterBuild1,
        __AfterBuild2.name() : __AfterBuild2
    })
    _AttributeMap.update({
        
    })
_module_typeBindings.CTD_ANON_2 = CTD_ANON_2


# Complex type [anonymous] with content type ELEMENT_ONLY
class CTD_ANON_3 (pyxb.binding.basis.complexTypeDefinition):
    """Complex type [anonymous] with content type ELEMENT_ONLY"""
    _TypeDefinition = None
    _ContentTypeTag = pyxb.binding.basis.complexTypeDefinition._CT_ELEMENT_ONLY
    _Abstract = False
    _ExpandedName = None
    _XSDLocation = pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 81, 10)
    _ElementMap = {}
    _AttributeMap = {}
    # Base type is pyxb.binding.datatypes.anyType
    
    # Element UseSimulator uses Python identifier UseSimulator
    __UseSimulator = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'UseSimulator'), 'UseSimulator', '__AbsentNamespace0_CTD_ANON_3_UseSimulator', False, pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 83, 14), )

    
    UseSimulator = property(__UseSimulator.value, __UseSimulator.set, None, None)

    
    # Element LoadApplicationAtStartup uses Python identifier LoadApplicationAtStartup
    __LoadApplicationAtStartup = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'LoadApplicationAtStartup'), 'LoadApplicationAtStartup', '__AbsentNamespace0_CTD_ANON_3_LoadApplicationAtStartup', False, pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 84, 14), )

    
    LoadApplicationAtStartup = property(__LoadApplicationAtStartup.value, __LoadApplicationAtStartup.set, None, None)

    
    # Element RunToMain uses Python identifier RunToMain
    __RunToMain = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'RunToMain'), 'RunToMain', '__AbsentNamespace0_CTD_ANON_3_RunToMain', False, pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 85, 14), )

    
    RunToMain = property(__RunToMain.value, __RunToMain.set, None, None)

    
    # Element InitializationFile uses Python identifier InitializationFile
    __InitializationFile = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'InitializationFile'), 'InitializationFile', '__AbsentNamespace0_CTD_ANON_3_InitializationFile', False, pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 86, 14), )

    
    InitializationFile = property(__InitializationFile.value, __InitializationFile.set, None, None)

    _ElementMap.update({
        __UseSimulator.name() : __UseSimulator,
        __LoadApplicationAtStartup.name() : __LoadApplicationAtStartup,
        __RunToMain.name() : __RunToMain,
        __InitializationFile.name() : __InitializationFile
    })
    _AttributeMap.update({
        
    })
_module_typeBindings.CTD_ANON_3 = CTD_ANON_3


# Complex type [anonymous] with content type ELEMENT_ONLY
class CTD_ANON_4 (pyxb.binding.basis.complexTypeDefinition):
    """Complex type [anonymous] with content type ELEMENT_ONLY"""
    _TypeDefinition = None
    _ContentTypeTag = pyxb.binding.basis.complexTypeDefinition._CT_ELEMENT_ONLY
    _Abstract = False
    _ExpandedName = None
    _XSDLocation = pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 91, 10)
    _ElementMap = {}
    _AttributeMap = {}
    # Base type is pyxb.binding.datatypes.anyType
    
    # Element UseTarget uses Python identifier UseTarget
    __UseTarget = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'UseTarget'), 'UseTarget', '__AbsentNamespace0_CTD_ANON_4_UseTarget', False, pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 93, 14), )

    
    UseTarget = property(__UseTarget.value, __UseTarget.set, None, None)

    
    # Element LoadApplicationAtStartup uses Python identifier LoadApplicationAtStartup
    __LoadApplicationAtStartup = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'LoadApplicationAtStartup'), 'LoadApplicationAtStartup', '__AbsentNamespace0_CTD_ANON_4_LoadApplicationAtStartup', False, pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 94, 14), )

    
    LoadApplicationAtStartup = property(__LoadApplicationAtStartup.value, __LoadApplicationAtStartup.set, None, None)

    
    # Element RunToMain uses Python identifier RunToMain
    __RunToMain = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'RunToMain'), 'RunToMain', '__AbsentNamespace0_CTD_ANON_4_RunToMain', False, pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 95, 14), )

    
    RunToMain = property(__RunToMain.value, __RunToMain.set, None, None)

    
    # Element InitializationFile uses Python identifier InitializationFile
    __InitializationFile = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'InitializationFile'), 'InitializationFile', '__AbsentNamespace0_CTD_ANON_4_InitializationFile', False, pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 96, 14), )

    
    InitializationFile = property(__InitializationFile.value, __InitializationFile.set, None, None)

    
    # Element Driver uses Python identifier Driver
    __Driver = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'Driver'), 'Driver', '__AbsentNamespace0_CTD_ANON_4_Driver', False, pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 98, 14), )

    
    Driver = property(__Driver.value, __Driver.set, None, None)

    _ElementMap.update({
        __UseTarget.name() : __UseTarget,
        __LoadApplicationAtStartup.name() : __LoadApplicationAtStartup,
        __RunToMain.name() : __RunToMain,
        __InitializationFile.name() : __InitializationFile,
        __Driver.name() : __Driver
    })
    _AttributeMap.update({
        
    })
_module_typeBindings.CTD_ANON_4 = CTD_ANON_4


# Complex type [anonymous] with content type ELEMENT_ONLY
class CTD_ANON_5 (pyxb.binding.basis.complexTypeDefinition):
    """Complex type [anonymous] with content type ELEMENT_ONLY"""
    _TypeDefinition = None
    _ContentTypeTag = pyxb.binding.basis.complexTypeDefinition._CT_ELEMENT_ONLY
    _Abstract = False
    _ExpandedName = None
    _XSDLocation = pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 105, 10)
    _ElementMap = {}
    _AttributeMap = {}
    # Base type is pyxb.binding.datatypes.anyType
    
    # Element Group uses Python identifier Group
    __Group = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'Group'), 'Group', '__AbsentNamespace0_CTD_ANON_5_Group', True, pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 107, 14), )

    
    Group = property(__Group.value, __Group.set, None, None)

    _ElementMap.update({
        __Group.name() : __Group
    })
    _AttributeMap.update({
        
    })
_module_typeBindings.CTD_ANON_5 = CTD_ANON_5


# Complex type [anonymous] with content type ELEMENT_ONLY
class CTD_ANON_6 (pyxb.binding.basis.complexTypeDefinition):
    """Complex type [anonymous] with content type ELEMENT_ONLY"""
    _TypeDefinition = None
    _ContentTypeTag = pyxb.binding.basis.complexTypeDefinition._CT_ELEMENT_ONLY
    _Abstract = False
    _ExpandedName = None
    _XSDLocation = pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 109, 16)
    _ElementMap = {}
    _AttributeMap = {}
    # Base type is pyxb.binding.datatypes.anyType
    
    # Element GroupName uses Python identifier GroupName
    __GroupName = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'GroupName'), 'GroupName', '__AbsentNamespace0_CTD_ANON_6_GroupName', False, pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 111, 20), )

    
    GroupName = property(__GroupName.value, __GroupName.set, None, None)

    
    # Element Files uses Python identifier Files
    __Files = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'Files'), 'Files', '__AbsentNamespace0_CTD_ANON_6_Files', False, pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 112, 20), )

    
    Files = property(__Files.value, __Files.set, None, None)

    _ElementMap.update({
        __GroupName.name() : __GroupName,
        __Files.name() : __Files
    })
    _AttributeMap.update({
        
    })
_module_typeBindings.CTD_ANON_6 = CTD_ANON_6


# Complex type [anonymous] with content type ELEMENT_ONLY
class CTD_ANON_7 (pyxb.binding.basis.complexTypeDefinition):
    """Complex type [anonymous] with content type ELEMENT_ONLY"""
    _TypeDefinition = None
    _ContentTypeTag = pyxb.binding.basis.complexTypeDefinition._CT_ELEMENT_ONLY
    _Abstract = False
    _ExpandedName = None
    _XSDLocation = pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 113, 22)
    _ElementMap = {}
    _AttributeMap = {}
    # Base type is pyxb.binding.datatypes.anyType
    
    # Element File uses Python identifier File
    __File = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'File'), 'File', '__AbsentNamespace0_CTD_ANON_7_File', True, pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 116, 26), )

    
    File = property(__File.value, __File.set, None, None)

    _ElementMap.update({
        __File.name() : __File
    })
    _AttributeMap.update({
        
    })
_module_typeBindings.CTD_ANON_7 = CTD_ANON_7


# Complex type [anonymous] with content type ELEMENT_ONLY
class CTD_ANON_8 (pyxb.binding.basis.complexTypeDefinition):
    """Complex type [anonymous] with content type ELEMENT_ONLY"""
    _TypeDefinition = None
    _ContentTypeTag = pyxb.binding.basis.complexTypeDefinition._CT_ELEMENT_ONLY
    _Abstract = False
    _ExpandedName = None
    _XSDLocation = pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 117, 28)
    _ElementMap = {}
    _AttributeMap = {}
    # Base type is pyxb.binding.datatypes.anyType
    
    # Element FileName uses Python identifier FileName
    __FileName = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'FileName'), 'FileName', '__AbsentNamespace0_CTD_ANON_8_FileName', False, pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 120, 32), )

    
    FileName = property(__FileName.value, __FileName.set, None, None)

    
    # Element FileType uses Python identifier FileType
    __FileType = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'FileType'), 'FileType', '__AbsentNamespace0_CTD_ANON_8_FileType', False, pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 122, 32), )

    
    FileType = property(__FileType.value, __FileType.set, None, None)

    
    # Element FilePath uses Python identifier FilePath
    __FilePath = pyxb.binding.content.ElementDeclaration(pyxb.namespace.ExpandedName(None, 'FilePath'), 'FilePath', '__AbsentNamespace0_CTD_ANON_8_FilePath', False, pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 124, 32), )

    
    FilePath = property(__FilePath.value, __FilePath.set, None, None)

    _ElementMap.update({
        __FileName.name() : __FileName,
        __FileType.name() : __FileType,
        __FilePath.name() : __FilePath
    })
    _AttributeMap.update({
        
    })
_module_typeBindings.CTD_ANON_8 = CTD_ANON_8


Project = pyxb.binding.basis.element(pyxb.namespace.ExpandedName(Namespace, 'Project'), CTD_ANON, location=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 25, 2))
Namespace.addCategoryObject('elementBinding', Project.name().localName(), Project)



CTD_ANON._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'ToolsetName'), pyxb.binding.datatypes.string, scope=CTD_ANON, location=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 29, 8)))

CTD_ANON._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'Device'), pyxb.binding.datatypes.string, scope=CTD_ANON, location=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 31, 8)))

CTD_ANON._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'Output'), CTD_ANON_, scope=CTD_ANON, location=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 33, 8)))

CTD_ANON._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'User'), CTD_ANON_2, scope=CTD_ANON, location=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 44, 8)))

CTD_ANON._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'IncludePath'), pyxb.binding.datatypes.string, scope=CTD_ANON, location=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 59, 8)))

CTD_ANON._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'ScatterFile'), pyxb.binding.datatypes.string, scope=CTD_ANON, location=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 61, 8)))

CTD_ANON._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'SFDFile'), pyxb.binding.datatypes.string, scope=CTD_ANON, location=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 63, 8)))

CTD_ANON._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'MiscControls'), pyxb.binding.datatypes.string, scope=CTD_ANON, location=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 65, 8)))

CTD_ANON._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'CDefines'), pyxb.binding.datatypes.string, scope=CTD_ANON, location=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 67, 8)))

CTD_ANON._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'AsmDefines'), pyxb.binding.datatypes.string, scope=CTD_ANON, location=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 69, 8)))

CTD_ANON._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'COptions'), pyxb.binding.datatypes.string, scope=CTD_ANON, location=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 71, 8)))

CTD_ANON._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'AsmOptions'), pyxb.binding.datatypes.string, scope=CTD_ANON, location=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 73, 8)))

CTD_ANON._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'LinkOptions'), pyxb.binding.datatypes.string, scope=CTD_ANON, location=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 75, 8)))

CTD_ANON._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'useUlib'), switcher, scope=CTD_ANON, location=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 77, 8)))

CTD_ANON._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'Simulator'), CTD_ANON_3, scope=CTD_ANON, location=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 80, 8)))

CTD_ANON._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'Target'), CTD_ANON_4, scope=CTD_ANON, location=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 90, 8)))

CTD_ANON._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'Groups'), CTD_ANON_5, scope=CTD_ANON, location=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 104, 8)))

def _BuildAutomaton_ ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_
    del _BuildAutomaton_
    import pyxb.utils.fac as fac

    counters = set()
    cc_0 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 29, 8))
    counters.add(cc_0)
    states = []
    final_update = set()
    final_update.add(fac.UpdateInstruction(cc_0, False))
    symbol = pyxb.binding.content.ElementUse(CTD_ANON._UseForTag(pyxb.namespace.ExpandedName(None, 'ToolsetName')), pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 29, 8))
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_0)
    transitions = []
    transitions.append(fac.Transition(st_0, [
        fac.UpdateInstruction(cc_0, True) ]))
    st_0._set_transitionSet(transitions)
    return fac.Automaton(states, counters, True, containing_state=st_0)

def _BuildAutomaton_2 ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_2
    del _BuildAutomaton_2
    import pyxb.utils.fac as fac

    counters = set()
    states = []
    final_update = set()
    symbol = pyxb.binding.content.ElementUse(CTD_ANON._UseForTag(pyxb.namespace.ExpandedName(None, 'Device')), pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 31, 8))
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_0)
    transitions = []
    st_0._set_transitionSet(transitions)
    return fac.Automaton(states, counters, False, containing_state=st_0)

def _BuildAutomaton_3 ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_3
    del _BuildAutomaton_3
    import pyxb.utils.fac as fac

    counters = set()
    cc_0 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 33, 8))
    counters.add(cc_0)
    states = []
    final_update = set()
    final_update.add(fac.UpdateInstruction(cc_0, False))
    symbol = pyxb.binding.content.ElementUse(CTD_ANON._UseForTag(pyxb.namespace.ExpandedName(None, 'Output')), pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 33, 8))
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_0)
    transitions = []
    transitions.append(fac.Transition(st_0, [
        fac.UpdateInstruction(cc_0, True) ]))
    st_0._set_transitionSet(transitions)
    return fac.Automaton(states, counters, True, containing_state=st_0)

def _BuildAutomaton_4 ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_4
    del _BuildAutomaton_4
    import pyxb.utils.fac as fac

    counters = set()
    cc_0 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 44, 8))
    counters.add(cc_0)
    states = []
    final_update = set()
    final_update.add(fac.UpdateInstruction(cc_0, False))
    symbol = pyxb.binding.content.ElementUse(CTD_ANON._UseForTag(pyxb.namespace.ExpandedName(None, 'User')), pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 44, 8))
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_0)
    transitions = []
    transitions.append(fac.Transition(st_0, [
        fac.UpdateInstruction(cc_0, True) ]))
    st_0._set_transitionSet(transitions)
    return fac.Automaton(states, counters, True, containing_state=st_0)

def _BuildAutomaton_5 ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_5
    del _BuildAutomaton_5
    import pyxb.utils.fac as fac

    counters = set()
    cc_0 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 59, 8))
    counters.add(cc_0)
    states = []
    final_update = set()
    final_update.add(fac.UpdateInstruction(cc_0, False))
    symbol = pyxb.binding.content.ElementUse(CTD_ANON._UseForTag(pyxb.namespace.ExpandedName(None, 'IncludePath')), pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 59, 8))
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_0)
    transitions = []
    transitions.append(fac.Transition(st_0, [
        fac.UpdateInstruction(cc_0, True) ]))
    st_0._set_transitionSet(transitions)
    return fac.Automaton(states, counters, True, containing_state=st_0)

def _BuildAutomaton_6 ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_6
    del _BuildAutomaton_6
    import pyxb.utils.fac as fac

    counters = set()
    cc_0 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 61, 8))
    counters.add(cc_0)
    states = []
    final_update = set()
    final_update.add(fac.UpdateInstruction(cc_0, False))
    symbol = pyxb.binding.content.ElementUse(CTD_ANON._UseForTag(pyxb.namespace.ExpandedName(None, 'ScatterFile')), pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 61, 8))
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_0)
    transitions = []
    transitions.append(fac.Transition(st_0, [
        fac.UpdateInstruction(cc_0, True) ]))
    st_0._set_transitionSet(transitions)
    return fac.Automaton(states, counters, True, containing_state=st_0)

def _BuildAutomaton_7 ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_7
    del _BuildAutomaton_7
    import pyxb.utils.fac as fac

    counters = set()
    cc_0 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 63, 8))
    counters.add(cc_0)
    states = []
    final_update = set()
    final_update.add(fac.UpdateInstruction(cc_0, False))
    symbol = pyxb.binding.content.ElementUse(CTD_ANON._UseForTag(pyxb.namespace.ExpandedName(None, 'SFDFile')), pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 63, 8))
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_0)
    transitions = []
    transitions.append(fac.Transition(st_0, [
        fac.UpdateInstruction(cc_0, True) ]))
    st_0._set_transitionSet(transitions)
    return fac.Automaton(states, counters, True, containing_state=st_0)

def _BuildAutomaton_8 ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_8
    del _BuildAutomaton_8
    import pyxb.utils.fac as fac

    counters = set()
    cc_0 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 65, 8))
    counters.add(cc_0)
    states = []
    final_update = set()
    final_update.add(fac.UpdateInstruction(cc_0, False))
    symbol = pyxb.binding.content.ElementUse(CTD_ANON._UseForTag(pyxb.namespace.ExpandedName(None, 'MiscControls')), pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 65, 8))
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_0)
    transitions = []
    transitions.append(fac.Transition(st_0, [
        fac.UpdateInstruction(cc_0, True) ]))
    st_0._set_transitionSet(transitions)
    return fac.Automaton(states, counters, True, containing_state=st_0)

def _BuildAutomaton_9 ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_9
    del _BuildAutomaton_9
    import pyxb.utils.fac as fac

    counters = set()
    cc_0 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 67, 8))
    counters.add(cc_0)
    states = []
    final_update = set()
    final_update.add(fac.UpdateInstruction(cc_0, False))
    symbol = pyxb.binding.content.ElementUse(CTD_ANON._UseForTag(pyxb.namespace.ExpandedName(None, 'CDefines')), pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 67, 8))
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_0)
    transitions = []
    transitions.append(fac.Transition(st_0, [
        fac.UpdateInstruction(cc_0, True) ]))
    st_0._set_transitionSet(transitions)
    return fac.Automaton(states, counters, True, containing_state=st_0)

def _BuildAutomaton_10 ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_10
    del _BuildAutomaton_10
    import pyxb.utils.fac as fac

    counters = set()
    cc_0 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 69, 8))
    counters.add(cc_0)
    states = []
    final_update = set()
    final_update.add(fac.UpdateInstruction(cc_0, False))
    symbol = pyxb.binding.content.ElementUse(CTD_ANON._UseForTag(pyxb.namespace.ExpandedName(None, 'AsmDefines')), pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 69, 8))
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_0)
    transitions = []
    transitions.append(fac.Transition(st_0, [
        fac.UpdateInstruction(cc_0, True) ]))
    st_0._set_transitionSet(transitions)
    return fac.Automaton(states, counters, True, containing_state=st_0)

def _BuildAutomaton_11 ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_11
    del _BuildAutomaton_11
    import pyxb.utils.fac as fac

    counters = set()
    cc_0 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 71, 8))
    counters.add(cc_0)
    states = []
    final_update = set()
    final_update.add(fac.UpdateInstruction(cc_0, False))
    symbol = pyxb.binding.content.ElementUse(CTD_ANON._UseForTag(pyxb.namespace.ExpandedName(None, 'COptions')), pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 71, 8))
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_0)
    transitions = []
    transitions.append(fac.Transition(st_0, [
        fac.UpdateInstruction(cc_0, True) ]))
    st_0._set_transitionSet(transitions)
    return fac.Automaton(states, counters, True, containing_state=st_0)

def _BuildAutomaton_12 ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_12
    del _BuildAutomaton_12
    import pyxb.utils.fac as fac

    counters = set()
    cc_0 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 73, 8))
    counters.add(cc_0)
    states = []
    final_update = set()
    final_update.add(fac.UpdateInstruction(cc_0, False))
    symbol = pyxb.binding.content.ElementUse(CTD_ANON._UseForTag(pyxb.namespace.ExpandedName(None, 'AsmOptions')), pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 73, 8))
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_0)
    transitions = []
    transitions.append(fac.Transition(st_0, [
        fac.UpdateInstruction(cc_0, True) ]))
    st_0._set_transitionSet(transitions)
    return fac.Automaton(states, counters, True, containing_state=st_0)

def _BuildAutomaton_13 ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_13
    del _BuildAutomaton_13
    import pyxb.utils.fac as fac

    counters = set()
    cc_0 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 75, 8))
    counters.add(cc_0)
    states = []
    final_update = set()
    final_update.add(fac.UpdateInstruction(cc_0, False))
    symbol = pyxb.binding.content.ElementUse(CTD_ANON._UseForTag(pyxb.namespace.ExpandedName(None, 'LinkOptions')), pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 75, 8))
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_0)
    transitions = []
    transitions.append(fac.Transition(st_0, [
        fac.UpdateInstruction(cc_0, True) ]))
    st_0._set_transitionSet(transitions)
    return fac.Automaton(states, counters, True, containing_state=st_0)

def _BuildAutomaton_14 ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_14
    del _BuildAutomaton_14
    import pyxb.utils.fac as fac

    counters = set()
    cc_0 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 77, 8))
    counters.add(cc_0)
    states = []
    final_update = set()
    final_update.add(fac.UpdateInstruction(cc_0, False))
    symbol = pyxb.binding.content.ElementUse(CTD_ANON._UseForTag(pyxb.namespace.ExpandedName(None, 'useUlib')), pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 77, 8))
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_0)
    transitions = []
    transitions.append(fac.Transition(st_0, [
        fac.UpdateInstruction(cc_0, True) ]))
    st_0._set_transitionSet(transitions)
    return fac.Automaton(states, counters, True, containing_state=st_0)

def _BuildAutomaton_15 ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_15
    del _BuildAutomaton_15
    import pyxb.utils.fac as fac

    counters = set()
    cc_0 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 80, 8))
    counters.add(cc_0)
    states = []
    final_update = set()
    final_update.add(fac.UpdateInstruction(cc_0, False))
    symbol = pyxb.binding.content.ElementUse(CTD_ANON._UseForTag(pyxb.namespace.ExpandedName(None, 'Simulator')), pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 80, 8))
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_0)
    transitions = []
    transitions.append(fac.Transition(st_0, [
        fac.UpdateInstruction(cc_0, True) ]))
    st_0._set_transitionSet(transitions)
    return fac.Automaton(states, counters, True, containing_state=st_0)

def _BuildAutomaton_16 ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_16
    del _BuildAutomaton_16
    import pyxb.utils.fac as fac

    counters = set()
    cc_0 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 90, 8))
    counters.add(cc_0)
    states = []
    final_update = set()
    final_update.add(fac.UpdateInstruction(cc_0, False))
    symbol = pyxb.binding.content.ElementUse(CTD_ANON._UseForTag(pyxb.namespace.ExpandedName(None, 'Target')), pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 90, 8))
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_0)
    transitions = []
    transitions.append(fac.Transition(st_0, [
        fac.UpdateInstruction(cc_0, True) ]))
    st_0._set_transitionSet(transitions)
    return fac.Automaton(states, counters, True, containing_state=st_0)

def _BuildAutomaton_17 ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_17
    del _BuildAutomaton_17
    import pyxb.utils.fac as fac

    counters = set()
    cc_0 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 104, 8))
    counters.add(cc_0)
    states = []
    final_update = set()
    final_update.add(fac.UpdateInstruction(cc_0, False))
    symbol = pyxb.binding.content.ElementUse(CTD_ANON._UseForTag(pyxb.namespace.ExpandedName(None, 'Groups')), pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 104, 8))
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_0)
    transitions = []
    transitions.append(fac.Transition(st_0, [
        fac.UpdateInstruction(cc_0, True) ]))
    st_0._set_transitionSet(transitions)
    return fac.Automaton(states, counters, True, containing_state=st_0)

def _BuildAutomaton ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton
    del _BuildAutomaton
    import pyxb.utils.fac as fac

    counters = set()
    cc_0 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 29, 8))
    counters.add(cc_0)
    cc_1 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 33, 8))
    counters.add(cc_1)
    cc_2 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 44, 8))
    counters.add(cc_2)
    cc_3 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 59, 8))
    counters.add(cc_3)
    cc_4 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 61, 8))
    counters.add(cc_4)
    cc_5 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 63, 8))
    counters.add(cc_5)
    cc_6 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 65, 8))
    counters.add(cc_6)
    cc_7 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 67, 8))
    counters.add(cc_7)
    cc_8 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 69, 8))
    counters.add(cc_8)
    cc_9 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 71, 8))
    counters.add(cc_9)
    cc_10 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 73, 8))
    counters.add(cc_10)
    cc_11 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 75, 8))
    counters.add(cc_11)
    cc_12 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 77, 8))
    counters.add(cc_12)
    cc_13 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 80, 8))
    counters.add(cc_13)
    cc_14 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 90, 8))
    counters.add(cc_14)
    cc_15 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 104, 8))
    counters.add(cc_15)
    states = []
    sub_automata = []
    sub_automata.append(_BuildAutomaton_())
    sub_automata.append(_BuildAutomaton_2())
    sub_automata.append(_BuildAutomaton_3())
    sub_automata.append(_BuildAutomaton_4())
    sub_automata.append(_BuildAutomaton_5())
    sub_automata.append(_BuildAutomaton_6())
    sub_automata.append(_BuildAutomaton_7())
    sub_automata.append(_BuildAutomaton_8())
    sub_automata.append(_BuildAutomaton_9())
    sub_automata.append(_BuildAutomaton_10())
    sub_automata.append(_BuildAutomaton_11())
    sub_automata.append(_BuildAutomaton_12())
    sub_automata.append(_BuildAutomaton_13())
    sub_automata.append(_BuildAutomaton_14())
    sub_automata.append(_BuildAutomaton_15())
    sub_automata.append(_BuildAutomaton_16())
    sub_automata.append(_BuildAutomaton_17())
    final_update = set()
    symbol = pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 27, 6)
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=True)
    st_0._set_subAutomata(*sub_automata)
    states.append(st_0)
    transitions = []
    st_0._set_transitionSet(transitions)
    return fac.Automaton(states, counters, False, containing_state=None)
CTD_ANON._Automaton = _BuildAutomaton()




CTD_ANON_2._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'BeforeCompile1'), UserAction, scope=CTD_ANON_2, location=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 48, 14)))

CTD_ANON_2._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'BeforeCompile2'), UserAction, scope=CTD_ANON_2, location=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 49, 14)))

CTD_ANON_2._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'BeforeBuild1'), UserAction, scope=CTD_ANON_2, location=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 50, 14)))

CTD_ANON_2._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'BeforeBuild2'), UserAction, scope=CTD_ANON_2, location=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 51, 14)))

CTD_ANON_2._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'AfterBuild1'), UserAction, scope=CTD_ANON_2, location=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 52, 14)))

CTD_ANON_2._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'AfterBuild2'), UserAction, scope=CTD_ANON_2, location=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 53, 14)))

def _BuildAutomaton_19 ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_19
    del _BuildAutomaton_19
    import pyxb.utils.fac as fac

    counters = set()
    cc_0 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 48, 14))
    counters.add(cc_0)
    states = []
    final_update = set()
    final_update.add(fac.UpdateInstruction(cc_0, False))
    symbol = pyxb.binding.content.ElementUse(CTD_ANON_2._UseForTag(pyxb.namespace.ExpandedName(None, 'BeforeCompile1')), pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 48, 14))
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_0)
    transitions = []
    transitions.append(fac.Transition(st_0, [
        fac.UpdateInstruction(cc_0, True) ]))
    st_0._set_transitionSet(transitions)
    return fac.Automaton(states, counters, True, containing_state=st_0)

def _BuildAutomaton_20 ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_20
    del _BuildAutomaton_20
    import pyxb.utils.fac as fac

    counters = set()
    cc_0 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 49, 14))
    counters.add(cc_0)
    states = []
    final_update = set()
    final_update.add(fac.UpdateInstruction(cc_0, False))
    symbol = pyxb.binding.content.ElementUse(CTD_ANON_2._UseForTag(pyxb.namespace.ExpandedName(None, 'BeforeCompile2')), pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 49, 14))
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_0)
    transitions = []
    transitions.append(fac.Transition(st_0, [
        fac.UpdateInstruction(cc_0, True) ]))
    st_0._set_transitionSet(transitions)
    return fac.Automaton(states, counters, True, containing_state=st_0)

def _BuildAutomaton_21 ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_21
    del _BuildAutomaton_21
    import pyxb.utils.fac as fac

    counters = set()
    cc_0 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 50, 14))
    counters.add(cc_0)
    states = []
    final_update = set()
    final_update.add(fac.UpdateInstruction(cc_0, False))
    symbol = pyxb.binding.content.ElementUse(CTD_ANON_2._UseForTag(pyxb.namespace.ExpandedName(None, 'BeforeBuild1')), pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 50, 14))
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_0)
    transitions = []
    transitions.append(fac.Transition(st_0, [
        fac.UpdateInstruction(cc_0, True) ]))
    st_0._set_transitionSet(transitions)
    return fac.Automaton(states, counters, True, containing_state=st_0)

def _BuildAutomaton_22 ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_22
    del _BuildAutomaton_22
    import pyxb.utils.fac as fac

    counters = set()
    cc_0 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 51, 14))
    counters.add(cc_0)
    states = []
    final_update = set()
    final_update.add(fac.UpdateInstruction(cc_0, False))
    symbol = pyxb.binding.content.ElementUse(CTD_ANON_2._UseForTag(pyxb.namespace.ExpandedName(None, 'BeforeBuild2')), pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 51, 14))
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_0)
    transitions = []
    transitions.append(fac.Transition(st_0, [
        fac.UpdateInstruction(cc_0, True) ]))
    st_0._set_transitionSet(transitions)
    return fac.Automaton(states, counters, True, containing_state=st_0)

def _BuildAutomaton_23 ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_23
    del _BuildAutomaton_23
    import pyxb.utils.fac as fac

    counters = set()
    cc_0 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 52, 14))
    counters.add(cc_0)
    states = []
    final_update = set()
    final_update.add(fac.UpdateInstruction(cc_0, False))
    symbol = pyxb.binding.content.ElementUse(CTD_ANON_2._UseForTag(pyxb.namespace.ExpandedName(None, 'AfterBuild1')), pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 52, 14))
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_0)
    transitions = []
    transitions.append(fac.Transition(st_0, [
        fac.UpdateInstruction(cc_0, True) ]))
    st_0._set_transitionSet(transitions)
    return fac.Automaton(states, counters, True, containing_state=st_0)

def _BuildAutomaton_24 ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_24
    del _BuildAutomaton_24
    import pyxb.utils.fac as fac

    counters = set()
    cc_0 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 53, 14))
    counters.add(cc_0)
    states = []
    final_update = set()
    final_update.add(fac.UpdateInstruction(cc_0, False))
    symbol = pyxb.binding.content.ElementUse(CTD_ANON_2._UseForTag(pyxb.namespace.ExpandedName(None, 'AfterBuild2')), pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 53, 14))
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_0)
    transitions = []
    transitions.append(fac.Transition(st_0, [
        fac.UpdateInstruction(cc_0, True) ]))
    st_0._set_transitionSet(transitions)
    return fac.Automaton(states, counters, True, containing_state=st_0)

def _BuildAutomaton_18 ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_18
    del _BuildAutomaton_18
    import pyxb.utils.fac as fac

    counters = set()
    cc_0 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 48, 14))
    counters.add(cc_0)
    cc_1 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 49, 14))
    counters.add(cc_1)
    cc_2 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 50, 14))
    counters.add(cc_2)
    cc_3 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 51, 14))
    counters.add(cc_3)
    cc_4 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 52, 14))
    counters.add(cc_4)
    cc_5 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 53, 14))
    counters.add(cc_5)
    states = []
    sub_automata = []
    sub_automata.append(_BuildAutomaton_19())
    sub_automata.append(_BuildAutomaton_20())
    sub_automata.append(_BuildAutomaton_21())
    sub_automata.append(_BuildAutomaton_22())
    sub_automata.append(_BuildAutomaton_23())
    sub_automata.append(_BuildAutomaton_24())
    final_update = set()
    symbol = pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 46, 12)
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=True)
    st_0._set_subAutomata(*sub_automata)
    states.append(st_0)
    transitions = []
    st_0._set_transitionSet(transitions)
    return fac.Automaton(states, counters, True, containing_state=None)
CTD_ANON_2._Automaton = _BuildAutomaton_18()




CTD_ANON_3._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'UseSimulator'), switcher, scope=CTD_ANON_3, location=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 83, 14)))

CTD_ANON_3._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'LoadApplicationAtStartup'), switcher, scope=CTD_ANON_3, location=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 84, 14)))

CTD_ANON_3._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'RunToMain'), switcher, scope=CTD_ANON_3, location=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 85, 14)))

CTD_ANON_3._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'InitializationFile'), pyxb.binding.datatypes.string, scope=CTD_ANON_3, location=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 86, 14)))

def _BuildAutomaton_26 ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_26
    del _BuildAutomaton_26
    import pyxb.utils.fac as fac

    counters = set()
    cc_0 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 83, 14))
    counters.add(cc_0)
    states = []
    final_update = set()
    final_update.add(fac.UpdateInstruction(cc_0, False))
    symbol = pyxb.binding.content.ElementUse(CTD_ANON_3._UseForTag(pyxb.namespace.ExpandedName(None, 'UseSimulator')), pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 83, 14))
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_0)
    transitions = []
    transitions.append(fac.Transition(st_0, [
        fac.UpdateInstruction(cc_0, True) ]))
    st_0._set_transitionSet(transitions)
    return fac.Automaton(states, counters, True, containing_state=st_0)

def _BuildAutomaton_27 ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_27
    del _BuildAutomaton_27
    import pyxb.utils.fac as fac

    counters = set()
    cc_0 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 84, 14))
    counters.add(cc_0)
    states = []
    final_update = set()
    final_update.add(fac.UpdateInstruction(cc_0, False))
    symbol = pyxb.binding.content.ElementUse(CTD_ANON_3._UseForTag(pyxb.namespace.ExpandedName(None, 'LoadApplicationAtStartup')), pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 84, 14))
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_0)
    transitions = []
    transitions.append(fac.Transition(st_0, [
        fac.UpdateInstruction(cc_0, True) ]))
    st_0._set_transitionSet(transitions)
    return fac.Automaton(states, counters, True, containing_state=st_0)

def _BuildAutomaton_28 ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_28
    del _BuildAutomaton_28
    import pyxb.utils.fac as fac

    counters = set()
    cc_0 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 85, 14))
    counters.add(cc_0)
    states = []
    final_update = set()
    final_update.add(fac.UpdateInstruction(cc_0, False))
    symbol = pyxb.binding.content.ElementUse(CTD_ANON_3._UseForTag(pyxb.namespace.ExpandedName(None, 'RunToMain')), pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 85, 14))
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_0)
    transitions = []
    transitions.append(fac.Transition(st_0, [
        fac.UpdateInstruction(cc_0, True) ]))
    st_0._set_transitionSet(transitions)
    return fac.Automaton(states, counters, True, containing_state=st_0)

def _BuildAutomaton_29 ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_29
    del _BuildAutomaton_29
    import pyxb.utils.fac as fac

    counters = set()
    cc_0 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 86, 14))
    counters.add(cc_0)
    states = []
    final_update = set()
    final_update.add(fac.UpdateInstruction(cc_0, False))
    symbol = pyxb.binding.content.ElementUse(CTD_ANON_3._UseForTag(pyxb.namespace.ExpandedName(None, 'InitializationFile')), pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 86, 14))
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_0)
    transitions = []
    transitions.append(fac.Transition(st_0, [
        fac.UpdateInstruction(cc_0, True) ]))
    st_0._set_transitionSet(transitions)
    return fac.Automaton(states, counters, True, containing_state=st_0)

def _BuildAutomaton_25 ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_25
    del _BuildAutomaton_25
    import pyxb.utils.fac as fac

    counters = set()
    cc_0 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 83, 14))
    counters.add(cc_0)
    cc_1 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 84, 14))
    counters.add(cc_1)
    cc_2 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 85, 14))
    counters.add(cc_2)
    cc_3 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 86, 14))
    counters.add(cc_3)
    states = []
    sub_automata = []
    sub_automata.append(_BuildAutomaton_26())
    sub_automata.append(_BuildAutomaton_27())
    sub_automata.append(_BuildAutomaton_28())
    sub_automata.append(_BuildAutomaton_29())
    final_update = set()
    symbol = pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 82, 12)
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=True)
    st_0._set_subAutomata(*sub_automata)
    states.append(st_0)
    transitions = []
    st_0._set_transitionSet(transitions)
    return fac.Automaton(states, counters, True, containing_state=None)
CTD_ANON_3._Automaton = _BuildAutomaton_25()




CTD_ANON_4._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'UseTarget'), switcher, scope=CTD_ANON_4, location=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 93, 14)))

CTD_ANON_4._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'LoadApplicationAtStartup'), switcher, scope=CTD_ANON_4, location=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 94, 14)))

CTD_ANON_4._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'RunToMain'), switcher, scope=CTD_ANON_4, location=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 95, 14)))

CTD_ANON_4._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'InitializationFile'), pyxb.binding.datatypes.string, scope=CTD_ANON_4, location=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 96, 14)))

CTD_ANON_4._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'Driver'), pyxb.binding.datatypes.string, scope=CTD_ANON_4, location=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 98, 14)))

def _BuildAutomaton_31 ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_31
    del _BuildAutomaton_31
    import pyxb.utils.fac as fac

    counters = set()
    cc_0 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 93, 14))
    counters.add(cc_0)
    states = []
    final_update = set()
    final_update.add(fac.UpdateInstruction(cc_0, False))
    symbol = pyxb.binding.content.ElementUse(CTD_ANON_4._UseForTag(pyxb.namespace.ExpandedName(None, 'UseTarget')), pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 93, 14))
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_0)
    transitions = []
    transitions.append(fac.Transition(st_0, [
        fac.UpdateInstruction(cc_0, True) ]))
    st_0._set_transitionSet(transitions)
    return fac.Automaton(states, counters, True, containing_state=st_0)

def _BuildAutomaton_32 ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_32
    del _BuildAutomaton_32
    import pyxb.utils.fac as fac

    counters = set()
    cc_0 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 94, 14))
    counters.add(cc_0)
    states = []
    final_update = set()
    final_update.add(fac.UpdateInstruction(cc_0, False))
    symbol = pyxb.binding.content.ElementUse(CTD_ANON_4._UseForTag(pyxb.namespace.ExpandedName(None, 'LoadApplicationAtStartup')), pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 94, 14))
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_0)
    transitions = []
    transitions.append(fac.Transition(st_0, [
        fac.UpdateInstruction(cc_0, True) ]))
    st_0._set_transitionSet(transitions)
    return fac.Automaton(states, counters, True, containing_state=st_0)

def _BuildAutomaton_33 ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_33
    del _BuildAutomaton_33
    import pyxb.utils.fac as fac

    counters = set()
    cc_0 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 95, 14))
    counters.add(cc_0)
    states = []
    final_update = set()
    final_update.add(fac.UpdateInstruction(cc_0, False))
    symbol = pyxb.binding.content.ElementUse(CTD_ANON_4._UseForTag(pyxb.namespace.ExpandedName(None, 'RunToMain')), pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 95, 14))
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_0)
    transitions = []
    transitions.append(fac.Transition(st_0, [
        fac.UpdateInstruction(cc_0, True) ]))
    st_0._set_transitionSet(transitions)
    return fac.Automaton(states, counters, True, containing_state=st_0)

def _BuildAutomaton_34 ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_34
    del _BuildAutomaton_34
    import pyxb.utils.fac as fac

    counters = set()
    cc_0 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 96, 14))
    counters.add(cc_0)
    states = []
    final_update = set()
    final_update.add(fac.UpdateInstruction(cc_0, False))
    symbol = pyxb.binding.content.ElementUse(CTD_ANON_4._UseForTag(pyxb.namespace.ExpandedName(None, 'InitializationFile')), pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 96, 14))
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_0)
    transitions = []
    transitions.append(fac.Transition(st_0, [
        fac.UpdateInstruction(cc_0, True) ]))
    st_0._set_transitionSet(transitions)
    return fac.Automaton(states, counters, True, containing_state=st_0)

def _BuildAutomaton_35 ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_35
    del _BuildAutomaton_35
    import pyxb.utils.fac as fac

    counters = set()
    cc_0 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 98, 14))
    counters.add(cc_0)
    states = []
    final_update = set()
    final_update.add(fac.UpdateInstruction(cc_0, False))
    symbol = pyxb.binding.content.ElementUse(CTD_ANON_4._UseForTag(pyxb.namespace.ExpandedName(None, 'Driver')), pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 98, 14))
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_0)
    transitions = []
    transitions.append(fac.Transition(st_0, [
        fac.UpdateInstruction(cc_0, True) ]))
    st_0._set_transitionSet(transitions)
    return fac.Automaton(states, counters, True, containing_state=st_0)

def _BuildAutomaton_30 ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_30
    del _BuildAutomaton_30
    import pyxb.utils.fac as fac

    counters = set()
    cc_0 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 93, 14))
    counters.add(cc_0)
    cc_1 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 94, 14))
    counters.add(cc_1)
    cc_2 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 95, 14))
    counters.add(cc_2)
    cc_3 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 96, 14))
    counters.add(cc_3)
    cc_4 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 98, 14))
    counters.add(cc_4)
    states = []
    sub_automata = []
    sub_automata.append(_BuildAutomaton_31())
    sub_automata.append(_BuildAutomaton_32())
    sub_automata.append(_BuildAutomaton_33())
    sub_automata.append(_BuildAutomaton_34())
    sub_automata.append(_BuildAutomaton_35())
    final_update = set()
    symbol = pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 92, 12)
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=True)
    st_0._set_subAutomata(*sub_automata)
    states.append(st_0)
    transitions = []
    st_0._set_transitionSet(transitions)
    return fac.Automaton(states, counters, True, containing_state=None)
CTD_ANON_4._Automaton = _BuildAutomaton_30()




CTD_ANON_5._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'Group'), CTD_ANON_6, scope=CTD_ANON_5, location=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 107, 14)))

def _BuildAutomaton_36 ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_36
    del _BuildAutomaton_36
    import pyxb.utils.fac as fac

    counters = set()
    states = []
    final_update = set()
    symbol = pyxb.binding.content.ElementUse(CTD_ANON_5._UseForTag(pyxb.namespace.ExpandedName(None, 'Group')), pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 107, 14))
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_0)
    transitions = []
    transitions.append(fac.Transition(st_0, [
         ]))
    st_0._set_transitionSet(transitions)
    return fac.Automaton(states, counters, False, containing_state=None)
CTD_ANON_5._Automaton = _BuildAutomaton_36()




CTD_ANON_6._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'GroupName'), pyxb.binding.datatypes.string, scope=CTD_ANON_6, location=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 111, 20)))

CTD_ANON_6._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'Files'), CTD_ANON_7, scope=CTD_ANON_6, location=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 112, 20)))

def _BuildAutomaton_37 ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_37
    del _BuildAutomaton_37
    import pyxb.utils.fac as fac

    counters = set()
    cc_0 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 111, 20))
    counters.add(cc_0)
    cc_1 = fac.CounterCondition(min=0, max=1, metadata=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 112, 20))
    counters.add(cc_1)
    states = []
    final_update = set()
    final_update.add(fac.UpdateInstruction(cc_0, False))
    symbol = pyxb.binding.content.ElementUse(CTD_ANON_6._UseForTag(pyxb.namespace.ExpandedName(None, 'GroupName')), pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 111, 20))
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_0)
    final_update = set()
    final_update.add(fac.UpdateInstruction(cc_1, False))
    symbol = pyxb.binding.content.ElementUse(CTD_ANON_6._UseForTag(pyxb.namespace.ExpandedName(None, 'Files')), pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 112, 20))
    st_1 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_1)
    transitions = []
    transitions.append(fac.Transition(st_0, [
        fac.UpdateInstruction(cc_0, True) ]))
    transitions.append(fac.Transition(st_1, [
        fac.UpdateInstruction(cc_0, False) ]))
    st_0._set_transitionSet(transitions)
    transitions = []
    transitions.append(fac.Transition(st_1, [
        fac.UpdateInstruction(cc_1, True) ]))
    st_1._set_transitionSet(transitions)
    return fac.Automaton(states, counters, True, containing_state=None)
CTD_ANON_6._Automaton = _BuildAutomaton_37()




CTD_ANON_7._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'File'), CTD_ANON_8, scope=CTD_ANON_7, location=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 116, 26)))

def _BuildAutomaton_38 ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_38
    del _BuildAutomaton_38
    import pyxb.utils.fac as fac

    counters = set()
    states = []
    final_update = set()
    symbol = pyxb.binding.content.ElementUse(CTD_ANON_7._UseForTag(pyxb.namespace.ExpandedName(None, 'File')), pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 116, 26))
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_0)
    transitions = []
    transitions.append(fac.Transition(st_0, [
         ]))
    st_0._set_transitionSet(transitions)
    return fac.Automaton(states, counters, False, containing_state=None)
CTD_ANON_7._Automaton = _BuildAutomaton_38()




CTD_ANON_8._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'FileName'), pyxb.binding.datatypes.string, scope=CTD_ANON_8, location=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 120, 32)))

CTD_ANON_8._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'FileType'), pyxb.binding.datatypes.byte, scope=CTD_ANON_8, location=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 122, 32)))

CTD_ANON_8._AddElement(pyxb.binding.basis.element(pyxb.namespace.ExpandedName(None, 'FilePath'), pyxb.binding.datatypes.string, scope=CTD_ANON_8, location=pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 124, 32)))

def _BuildAutomaton_39 ():
    # Remove this helper function from the namespace after it is invoked
    global _BuildAutomaton_39
    del _BuildAutomaton_39
    import pyxb.utils.fac as fac

    counters = set()
    states = []
    final_update = None
    symbol = pyxb.binding.content.ElementUse(CTD_ANON_8._UseForTag(pyxb.namespace.ExpandedName(None, 'FileName')), pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 120, 32))
    st_0 = fac.State(symbol, is_initial=True, final_update=final_update, is_unordered_catenation=False)
    states.append(st_0)
    final_update = None
    symbol = pyxb.binding.content.ElementUse(CTD_ANON_8._UseForTag(pyxb.namespace.ExpandedName(None, 'FileType')), pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 122, 32))
    st_1 = fac.State(symbol, is_initial=False, final_update=final_update, is_unordered_catenation=False)
    states.append(st_1)
    final_update = set()
    symbol = pyxb.binding.content.ElementUse(CTD_ANON_8._UseForTag(pyxb.namespace.ExpandedName(None, 'FilePath')), pyxb.utils.utility.Location('C:\\Keil_v5\\UV4\\project_import.xsd', 124, 32))
    st_2 = fac.State(symbol, is_initial=False, final_update=final_update, is_unordered_catenation=False)
    states.append(st_2)
    transitions = []
    transitions.append(fac.Transition(st_1, [
         ]))
    st_0._set_transitionSet(transitions)
    transitions = []
    transitions.append(fac.Transition(st_2, [
         ]))
    st_1._set_transitionSet(transitions)
    transitions = []
    st_2._set_transitionSet(transitions)
    return fac.Automaton(states, counters, False, containing_state=None)
CTD_ANON_8._Automaton = _BuildAutomaton_39()

