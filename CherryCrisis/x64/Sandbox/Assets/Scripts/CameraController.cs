using CCEngine;
using System;

namespace CCScripting
{
	public class CameraController : Behaviour
	{
		public CameraController(System.IntPtr cPtr, bool cMemoryOwn)
		: base(cPtr, cMemoryOwn) { }

		Transform transform;
		Transform cubeOffset;

		PickableCube pickedCube;
		float throwingStrength = 5f;

		float speedSensivity = 80.0f;
		float interactRange = 25f;

		public void Awake() => transform = GetBehaviour<Transform>();

		public void Update()
		{
			if (transform == null) return;

			UpdateCameraRotation();

			if (InputManager.GetKeyDown(Keycode.E)) TryInteract();
			if (InputManager.GetKeyDown(Keycode.LEFT_CLICK)) TryAction();
		}

		// fuck interfaces
		void TryInteract()
		{
			// this needs to be improved, vectors seems to be wrongs
			RaycastHit hit = PhysicManager.Raycast(GetHost().m_cell, transform.GetGlobalPosition(), transform.Forward().Normalized(), interactRange);

			if (hit != null && hit.actor != null && hit.actor.m_owner != null)
			{
				Debug.Log(ELogType.INFO, "Hitted " + hit.actor.m_owner.name);

				PortalSwitcher switcher = hit.actor.m_owner.GetBehaviour<PortalSwitcher>();
				if (switcher != null) 
				{
					switcher.Switch();
					return;
				}

				Wardrobe wardrobe = hit.actor.m_owner.GetBehaviour<Wardrobe>();
				if (wardrobe != null)
				{
					wardrobe.SetInMovement();
					return;
				}

				PickableCube cube = hit.actor.m_owner.GetBehaviour<PickableCube>();
				if (cube != null) 
				{
					PickupCube(cube);
					return;
				}

				CubeSpawner spawner = hit.actor.m_owner.GetBehaviour<CubeSpawner>();
				if (spawner != null) 
				{
					spawner.SpawnCube();
					return;
				}
			}
		}

		void TryAction() 
		{
			if (pickedCube != null)
				ThrowCube();
		}

		void UpdateCameraRotation() 
		{
			Vector2 deltaMouse = InputManager.GetMouseDelta();
			float dt = Time.GetDeltaTime();

			float sensitityY = deltaMouse.y;
			double angleX = transform.eulerAngles.x + sensitityY * speedSensivity * 0.002f;
			angleX = Math.Min(Math.Max(angleX, -Math.PI * 0.4f), Math.PI * 0.4f);

			transform.eulerAngles = new Vector3((float)angleX, transform.eulerAngles.y, transform.eulerAngles.z);
		}

		void PickupCube(PickableCube cube) 
		{
			cube.Pick(cubeOffset);
			pickedCube = cube;
		}

		void ThrowCube()
		{
			pickedCube.Throw(transform.Forward().Normalized(), throwingStrength);
			pickedCube = null;
		}
	}
}