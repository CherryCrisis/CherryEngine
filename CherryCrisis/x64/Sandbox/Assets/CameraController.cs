using CCEngine;
using System;

namespace CCScripting
{
	public class CameraController : Behaviour
	{
		Transform transform;

		public CameraController(System.IntPtr cPtr, bool cMemoryOwn) 
			: base(cPtr, cMemoryOwn)
		{

		}

		public void Start()
		{
			transform = GetComponent<Transform>();

			Debug.GetInstance().Log(ELogType.WARNING, "Coucou Paul mets nous 20/20 (stp)");
			Debug.GetInstance().Log(ELogType.ERROR, "Failed to fail :)");
		}


		public void Update()
		{
			if (transform == null)
				return;
			
			Vector2 deltaMouse = InputManager.GetMouseDelta();
			float sensitityY = Time.GetDeltaTime() * deltaMouse.y;

			double angle = transform.eulerAngles.x + sensitityY;
			angle = Math.Min(Math.Max(angle, -Math.PI * 0.4f), Math.PI * 0.4f);

			transform.eulerAngles = new Vector3((float)angle, transform.eulerAngles.y, transform.eulerAngles.z);
		}

		public void OnCollisionEnter(Entity other)
        {
			Debug.GetInstance().Info($"{other}, in");
        }

		public void OnCollisionStay(Entity other)
		{
			Debug.GetInstance().Info($"{other}, stay");
		}

		public void OnCollisionExit(Entity other)
		{
			Debug.GetInstance().Info($"{other}, out");
		}

		public void OnTriggerEnter(Entity other)
		{
			Debug.GetInstance().Info($"{other}, in");
		}
	}
}
