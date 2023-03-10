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

public class LightComponentVector : global::System.IDisposable, global::System.Collections.IEnumerable, global::System.Collections.Generic.IList<LightComponent>
 {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  public LightComponentVector(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  public static global::System.Runtime.InteropServices.HandleRef getCPtr(LightComponentVector obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~LightComponentVector() {
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
          CherryEnginePINVOKE.delete_LightComponentVector(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
    }
  }

  public LightComponentVector(global::System.Collections.IEnumerable c) : this() {
    if (c == null)
      throw new global::System.ArgumentNullException("c");
    foreach (LightComponent element in c) {
      this.Add(element);
    }
  }

  public LightComponentVector(global::System.Collections.Generic.IEnumerable<LightComponent> c) : this() {
    if (c == null)
      throw new global::System.ArgumentNullException("c");
    foreach (LightComponent element in c) {
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

  public LightComponent this[int index]  {
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

  public void CopyTo(LightComponent[] array)
  {
    CopyTo(0, array, 0, this.Count);
  }

  public void CopyTo(LightComponent[] array, int arrayIndex)
  {
    CopyTo(0, array, arrayIndex, this.Count);
  }

  public void CopyTo(int index, LightComponent[] array, int arrayIndex, int count)
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

  public LightComponent[] ToArray() {
    LightComponent[] array = new LightComponent[this.Count];
    this.CopyTo(array);
    return array;
  }

  global::System.Collections.Generic.IEnumerator<LightComponent> global::System.Collections.Generic.IEnumerable<LightComponent>.GetEnumerator() {
    return new LightComponentVectorEnumerator(this);
  }

  global::System.Collections.IEnumerator global::System.Collections.IEnumerable.GetEnumerator() {
    return new LightComponentVectorEnumerator(this);
  }

  public LightComponentVectorEnumerator GetEnumerator() {
    return new LightComponentVectorEnumerator(this);
  }

  // Type-safe enumerator
  /// Note that the IEnumerator documentation requires an InvalidOperationException to be thrown
  /// whenever the collection is modified. This has been done for changes in the size of the
  /// collection but not when one of the elements of the collection is modified as it is a bit
  /// tricky to detect unmanaged code that modifies the collection under our feet.
  public sealed class LightComponentVectorEnumerator : global::System.Collections.IEnumerator
    , global::System.Collections.Generic.IEnumerator<LightComponent>
  {
    private LightComponentVector collectionRef;
    private int currentIndex;
    private object currentObject;
    private int currentSize;

    public LightComponentVectorEnumerator(LightComponentVector collection) {
      collectionRef = collection;
      currentIndex = -1;
      currentObject = null;
      currentSize = collectionRef.Count;
    }

    // Type-safe iterator Current
    public LightComponent Current {
      get {
        if (currentIndex == -1)
          throw new global::System.InvalidOperationException("Enumeration not started.");
        if (currentIndex > currentSize - 1)
          throw new global::System.InvalidOperationException("Enumeration finished.");
        if (currentObject == null)
          throw new global::System.InvalidOperationException("Collection modified.");
        return (LightComponent)currentObject;
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
    CherryEnginePINVOKE.LightComponentVector_Clear(swigCPtr);
  }

  public void Add(LightComponent x) {
    CherryEnginePINVOKE.LightComponentVector_Add(swigCPtr, LightComponent.getCPtr(x));
  }

  private uint size() {
    uint ret = CherryEnginePINVOKE.LightComponentVector_size(swigCPtr);
    return ret;
  }

  private uint capacity() {
    uint ret = CherryEnginePINVOKE.LightComponentVector_capacity(swigCPtr);
    return ret;
  }

  private void reserve(uint n) {
    CherryEnginePINVOKE.LightComponentVector_reserve(swigCPtr, n);
  }

  public LightComponentVector() : this(CherryEnginePINVOKE.new_LightComponentVector__SWIG_0(), true) {
  }

  public LightComponentVector(LightComponentVector other) : this(CherryEnginePINVOKE.new_LightComponentVector__SWIG_1(LightComponentVector.getCPtr(other)), true) {
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public LightComponentVector(int capacity) : this(CherryEnginePINVOKE.new_LightComponentVector__SWIG_2(capacity), true) {
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  private LightComponent getitemcopy(int index) {
    global::System.IntPtr cPtr = CherryEnginePINVOKE.LightComponentVector_getitemcopy(swigCPtr, index);
    LightComponent ret = (cPtr == global::System.IntPtr.Zero) ? null : new LightComponent(cPtr, false);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  private LightComponent getitem(int index) {
    global::System.IntPtr cPtr = CherryEnginePINVOKE.LightComponentVector_getitem(swigCPtr, index);
    LightComponent ret = (cPtr == global::System.IntPtr.Zero) ? null : new LightComponent(cPtr, false);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  private void setitem(int index, LightComponent val) {
    CherryEnginePINVOKE.LightComponentVector_setitem(swigCPtr, index, LightComponent.getCPtr(val));
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public void AddRange(LightComponentVector values) {
    CherryEnginePINVOKE.LightComponentVector_AddRange(swigCPtr, LightComponentVector.getCPtr(values));
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public LightComponentVector GetRange(int index, int count) {
    global::System.IntPtr cPtr = CherryEnginePINVOKE.LightComponentVector_GetRange(swigCPtr, index, count);
    LightComponentVector ret = (cPtr == global::System.IntPtr.Zero) ? null : new LightComponentVector(cPtr, true);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public void Insert(int index, LightComponent x) {
    CherryEnginePINVOKE.LightComponentVector_Insert(swigCPtr, index, LightComponent.getCPtr(x));
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public void InsertRange(int index, LightComponentVector values) {
    CherryEnginePINVOKE.LightComponentVector_InsertRange(swigCPtr, index, LightComponentVector.getCPtr(values));
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public void RemoveAt(int index) {
    CherryEnginePINVOKE.LightComponentVector_RemoveAt(swigCPtr, index);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public void RemoveRange(int index, int count) {
    CherryEnginePINVOKE.LightComponentVector_RemoveRange(swigCPtr, index, count);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public static LightComponentVector Repeat(LightComponent value, int count) {
    global::System.IntPtr cPtr = CherryEnginePINVOKE.LightComponentVector_Repeat(LightComponent.getCPtr(value), count);
    LightComponentVector ret = (cPtr == global::System.IntPtr.Zero) ? null : new LightComponentVector(cPtr, true);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public void Reverse() {
    CherryEnginePINVOKE.LightComponentVector_Reverse__SWIG_0(swigCPtr);
  }

  public void Reverse(int index, int count) {
    CherryEnginePINVOKE.LightComponentVector_Reverse__SWIG_1(swigCPtr, index, count);
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public void SetRange(int index, LightComponentVector values) {
    CherryEnginePINVOKE.LightComponentVector_SetRange(swigCPtr, index, LightComponentVector.getCPtr(values));
    if (CherryEnginePINVOKE.SWIGPendingException.Pending) throw CherryEnginePINVOKE.SWIGPendingException.Retrieve();
  }

  public bool Contains(LightComponent value) {
    bool ret = CherryEnginePINVOKE.LightComponentVector_Contains(swigCPtr, LightComponent.getCPtr(value));
    return ret;
  }

  public int IndexOf(LightComponent value) {
    int ret = CherryEnginePINVOKE.LightComponentVector_IndexOf(swigCPtr, LightComponent.getCPtr(value));
    return ret;
  }

  public int LastIndexOf(LightComponent value) {
    int ret = CherryEnginePINVOKE.LightComponentVector_LastIndexOf(swigCPtr, LightComponent.getCPtr(value));
    return ret;
  }

  public bool Remove(LightComponent value) {
    bool ret = CherryEnginePINVOKE.LightComponentVector_Remove(swigCPtr, LightComponent.getCPtr(value));
    return ret;
  }

}

}
