using CCEngine;

namespace CCScripting
{
	public class DebugTest : Behaviour
	{

		public DebugTest(System.IntPtr cPtr, bool cMemoryOwn) : base(cPtr, cMemoryOwn)
		{
		}

		void Start()
        {

		}
		int oui = 6;

		Vector3 vec = new Vector3(5, 7, 6);

		int SuperOui { set => oui = value; get => oui + 7; }

		public string error = "Je suis un message de debug moins utile !";
		public void Debugger()
		{
			Debug.GetInstance().Warning(vec);
			Debug.GetInstance().Error(error);
		}
	}
}
