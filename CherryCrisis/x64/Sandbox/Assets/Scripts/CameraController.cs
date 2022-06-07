using CCEngine;
using System;

namespace CCScripting
{
	public class CameraController : Behaviour
	{
		public CameraController(System.IntPtr cPtr, bool cMemoryOwn)
		: base(cPtr, cMemoryOwn) { }

		Transform transform;

		float speedSensivity = 80.0f;
		float interactRange  = 25f;

		public void Awake() => transform = GetBehaviour<Transform>();

		public void Update()
		{
			if (transform == null) return;

			UpdateCameraRotation();

			if (InputManager.GetKeyDown(Keycode.E)) TryInteract(); 
		}
		
		// fuck interfaces
		void TryInteract()  
		{
			// this needs to be improved, vectors seems to be wrongs
			RaycastHit hit = PhysicManager.Raycast(GetHost().m_cell, transform.GetGlobalPosition(), transform.Forward(), interactRange);
			
			if (hit != null && hit.actor != null &&hit.actor.m_owner != null)
			{
				Debug.Log(ELogType.INFO, "Hitted " + hit.actor.m_owner.name);

				PortalSwitcher switcher = hit.actor.m_owner.GetBehaviour<PortalSwitcher>();
				if (switcher != null)
					switcher.Switch();

				Wardrobe wardrobe = hit.actor.m_owner.GetBehaviour<Wardrobe>();
				if (wardrobe != null)
					wardrobe.SetInMovement();

			}
		}

		void UpdateCameraRotation() 
		{
			Vector2 deltaMouse = InputManager.GetMouseDelta();
			float dt = Time.GetDeltaTime();

			float sensitityY = deltaMouse.y;
			double angleX = transform.eulerAngles.x + sensitityY * speedSensivity * dt;
			angleX = Math.Min(Math.Max(angleX, -Math.PI * 0.4f), Math.PI * 0.4f);

			transform.eulerAngles = new Vector3((float)angleX, transform.eulerAngles.y, transform.eulerAngles.z);
		}
	}
}