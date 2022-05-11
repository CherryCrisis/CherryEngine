%module (directors="1") CherryEngine

SWIG_CSBODY_PROXY(public, public, SWIGTYPE)
SWIG_CSBODY_TYPEWRAPPER(public, public, public, SWIGTYPE)

%include input_manager.i
%include time_manager.i
%include debug.i

%include behaviour.i
%include entity.i
%include maths.i

