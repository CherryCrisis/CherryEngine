//------------------------------------------------------------------------------
// <auto-generated />
//
// This file was automatically generated by SWIG (http://www.swig.org).
// Version 4.0.2
//
// Do not make changes to this file unless you know what you are doing--modify
// the SWIG interface file instead.
//------------------------------------------------------------------------------

namespace CCEngine {

public class ModelRendererVector : global::System.IDisposable, global::System.Collections.IEnumerable, global::System.Collections.Generic.IList<ModelRenderer>
 {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  public ModelRendererVector(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  public static global::System.Runtime.InteropServices.HandleRef getCPtr(ModelRendererVector obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~ModelRendererVector() {
    Dispose(false);
  }

  public void Dispose() {
    Dispose(true);
    global::System.GC.SuppressFinalize(this);
  }

  protected virtual void Dispose(bool disposing) {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          CherryEnginePINVOKE.delete_ModelRendererVector(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
    }
  }

  public ModelRendererVector(global::System.Collections.IEnumerable c) : this() {
    if (c == null)
      throw new global::System.ArgumentNullException("c");
    foreach (ModelRenderer element in c) {
      this.Add(element);
    }
  }

  public ModelRendererVector(global::System.Collections.Generic.IEnumerable<ModelRenderer> c) : this() {
    if (c == null)
      throw new global::System.ArgumentNullException("c");
    foreach (ModelRenderer element in c) {
      this.Add(element);
    }
  }

  public bool IsFixedSize {
    get {
      return false;
    }
  }

  public bool IsReadOnly {
    get {
      return false;
    }
  }

  public ModelRenderer this[int index]  {
    get {
      return getitem(index);
    }
    set {
      setitem(index, value);
    }
  }

  public int Capacity {
    get {
      return (int)capacity();
    }
    set {
      if (value < size())
        throw new global::System.ArgumentOutOfRangeException("Capacity");
      reserve((uint)value);
    }
  }

  public int Count {
    get {
      return (int)size();
    }
  }

  public bool IsSynchronized {
    get {
      return false;
    }
  }

  public void CopyTo(ModelRenderer[] array)
  {
    CopyTo(0, array, 0, this.Count);
  }

  public void CopyTo(ModelRenderer[] array, int arrayIndex)
  {
    CopyTo(0, array, arrayIndex, this.Count);
  }

  public void CopyTo(int index, ModelRenderer[] array, int arrayIndex, int count)
  {
    if (array == null)
      throw new global::System.ArgumentNullException("array");
    if (index < 0)
      throw new global::System.ArgumentOutOfRangeException("index", "Value is less than zero");
    if (arrayIndex < 0)
      throw new global::System.ArgumentOutOfRangeException("arrayIndex", "Value is less than zero");
    if (count < 0)
      throw new global::System.ArgumentOutOfRangeException("count", "Value is less than zero");
    if (array.Rank > 1)
      throw new global::System.ArgumentException("Multi dimensional array.", "array");
    if (index+count > this.Count || arrayIndex+count > array.Length)
      throw new global::System.ArgumentException("Number of elements to copy is too large.");
    for (int i=0; i<count; i++)
      array.SetValue(getitemcopy(index+i), arrayIndex+i);
  }

  public ModelRenderer[] ToArray() {
    ModelRenderer[] array = new ModelRenderer[this.Count];
    this.CopyTo(array);
    return array;
  }

  global::System.Collections.Generic.IEnumerator<ModelRenderer> global::System.Collections.Generic.IEnumerable<ModelRenderer>.GetEnumerator() {
    return new ModelRendererVectorEnumerator(this);
  }

  global::System.Collections.IEnumerator global::System.Collections.IEnumerable.GetEnumerator() {
    return new ModelRendererVectorEnumerator(this);
  }

  public ModelRendererVectorEnumerator GetEnumerator() {
    return new ModelRendererVectorEnumerator(this);
  }

  // Type-safe enumerator
  /// Note that the IEnumerator documentation requires an InvalidOperationException to be thrown
  /// whenever the collection is modified. This has been done for changes in the size of the
  /// collection but not when one of the elements of the collection is modified as it is a bit
  /// tricky to detect unmanaged code that modifies the collection under our feet.
  public sealed class ModelRendererVectorEnumerator : global::System.Collections.IEnumerator
    , global::System.Collections.Generic.IEnumerator<ModelRenderer>
  {
    private ModelRendererVector collectionRef;
    private int currentIndex;
    private object currentObject;
    private int currentSize;

    public ModelRendererVectorEnumerator(ModelRendererVector collection) {
      collectionRef = collection;
      currentIndex = -1;
      currentObject = null;
      currentSize = collectionRef.Count;
    }

    // Type-safe iterator Current
    public ModelRenderer Current {
      get {
        if (currentIndex == -1)
          throw new global::System.InvalidOperationException("Enumeration not started.");
        if (currentIndex > currentSize - 1)
          throw new global::System.InvalidOperationException("Enumeration finished.");
        if (currentObject == null)
          throw new global::System.InvalidOperationException("Collection modified.");
        return (ModelRenderer)currentObject;
      }
    }

    // Type-unsafe IEnumerator.Current
    object global::System.Collections.IEnumerator.Current {
      get {
        return Current;
      }
    }

    public bool MoveNext() {
      int size = collectionRef.Count;
      bool moveOkay = (currentIndex+1 < size) && (size == currentSize);
      if (moveOkay) {
        currentIndex++;
        currentObject = collectionRef[currentIndex];
      } else {
        currentObject = null;
      }
      return moveOkay;
    }

    public void Reset() {
      currentIndex = -1;
      currentObject = null;
      if (collectionRef.Count != currentSize) {
        throw new global::System.InvalidOperationException("Collection modified.");
      }
    }

    public void Dispose() {
        currentIndex = -1;
        currentObject = null;
    }
  }

  public void Clear() {
    CherryEnginePINVOKE.ModelRendererVector_Clear(swigCPtr);
  }

  public void Add(ModelRenderer x) {
    CherryEnginePINVOKE.ModelRendererVector_Add(swigCPtr, ModelRenderer.getCPtr(x));
  }

  private uint size() {
    uint ret = CherryEnginePINVOKE.ModelRendererVector_size(swigCPtr);
    return ret;
  }

  private uint capacity() {
    uint ret = CherryEnginePINVOKE.ModelRendererVector_capacity(swigCPtr);
    return ret;
  }

  private void reserve(uint n) {
    CherryEnginePINVOKE.ModelRendererVector_reserve(swigCPtr, n);
  }

  public ModelRendererVector() : this(CherryEnginePINVOKE.new_ModelRendererVector__SWIG_0(), true) {
  }

  public ModelRendererVector(ModelRendererVector other) : this(CherryEnginePINVOKE.new_ModelRendererVector__SWIG_1(ModelRendererVector.getCPtr(other)), true) {
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public ModelRendererVector(int capacity) : this(CherryEnginePINVOKE.new_ModelRendererVector__SWIG_2(capacity), true) {
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  private ModelRenderer getitemcopy(int index) {
    global::System.IntPtr cPtr = CherryEnginePINVOKE.ModelRendererVector_getitemcopy(swigCPtr, index);
    ModelRenderer ret = (cPtr == global::System.IntPtr.Zero) ? null : new ModelRenderer(cPtr, false);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  private ModelRenderer getitem(int index) {
    global::System.IntPtr cPtr = CherryEnginePINVOKE.ModelRendererVector_getitem(swigCPtr, index);
    ModelRenderer ret = (cPtr == global::System.IntPtr.Zero) ? null : new ModelRenderer(cPtr, false);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  private void setitem(int index, ModelRenderer val) {
    CherryEnginePINVOKE.ModelRendererVector_setitem(swigCPtr, index, ModelRenderer.getCPtr(val));
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public void AddRange(ModelRendererVector values) {
    CherryEnginePINVOKE.ModelRendererVector_AddRange(swigCPtr, ModelRendererVector.getCPtr(values));
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public ModelRendererVector GetRange(int index, int count) {
    global::System.IntPtr cPtr = CherryEnginePINVOKE.ModelRendererVector_GetRange(swigCPtr, index, count);
    ModelRendererVector ret = (cPtr == global::System.IntPtr.Zero) ? null : new ModelRendererVector(cPtr, true);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public void Insert(int index, ModelRenderer x) {
    CherryEnginePINVOKE.ModelRendererVector_Insert(swigCPtr, index, ModelRenderer.getCPtr(x));
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public void InsertRange(int index, ModelRendererVector values) {
    CherryEnginePINVOKE.ModelRendererVector_InsertRange(swigCPtr, index, ModelRendererVector.getCPtr(values));
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public void RemoveAt(int index) {
    CherryEnginePINVOKE.ModelRendererVector_RemoveAt(swigCPtr, index);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public void RemoveRange(int index, int count) {
    CherryEnginePINVOKE.ModelRendererVector_RemoveRange(swigCPtr, index, count);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public static ModelRendererVector Repeat(ModelRenderer value, int count) {
    global::System.IntPtr cPtr = CherryEnginePINVOKE.ModelRendererVector_Repeat(ModelRenderer.getCPtr(value), count);
    ModelRendererVector ret = (cPtr == global::System.IntPtr.Zero) ? null : new ModelRendererVector(cPtr, true);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public void Reverse() {
    CherryEnginePINVOKE.ModelRendererVector_Reverse__SWIG_0(swigCPtr);
  }

  public void Reverse(int index, int count) {
    CherryEnginePINVOKE.ModelRendererVector_Reverse__SWIG_1(swigCPtr, index, count);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public void SetRange(int index, ModelRendererVector values) {
    CherryEnginePINVOKE.ModelRendererVector_SetRange(swigCPtr, index, ModelRendererVector.getCPtr(values));
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public bool Contains(ModelRenderer value) {
    bool ret = CherryEnginePINVOKE.ModelRendererVector_Contains(swigCPtr, ModelRenderer.getCPtr(value));
    return ret;
  }

  public int IndexOf(ModelRenderer value) {
    int ret = CherryEnginePINVOKE.ModelRendererVector_IndexOf(swigCPtr, ModelRenderer.getCPtr(value));
    return ret;
  }

  public int LastIndexOf(ModelRenderer value) {
    int ret = CherryEnginePINVOKE.ModelRendererVector_LastIndexOf(swigCPtr, ModelRenderer.getCPtr(value));
    return ret;
  }

  public bool Remove(ModelRenderer value) {
    bool ret = CherryEnginePINVOKE.ModelRendererVector_Remove(swigCPtr, ModelRenderer.getCPtr(value));
    return ret;
  }

}

}
