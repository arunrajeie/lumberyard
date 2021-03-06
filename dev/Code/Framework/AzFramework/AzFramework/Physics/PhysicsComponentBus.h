/*
* All or portions of this file Copyright (c) Amazon.com, Inc. or its affiliates or
* its licensors.
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
* or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/
#pragma once

#include <AzCore/Component/ComponentBus.h>
#include <AzCore/Math/Quaternion.h>
#include <AzCore/Math/Aabb.h>
#include <AzCore/Memory/SystemAllocator.h>

#include <AzCore/std/containers/array.h>

struct IPhysicalEntity;
struct pe_params;
struct pe_action;
struct pe_status;

namespace Physics
{
    class RigidBody;
}

namespace AzFramework
{
    /*!
     * Messages serviced by the in-game physics component.
     */
    class PhysicsComponentRequests
        : public AZ::ComponentBus
    {
    public:

        virtual ~PhysicsComponentRequests() {}

        //! Make the entity a participant in the physics simulation.
        //! This is an asynchronous request.
        //! The PhysicsComponentNotificationBus::OnPhysicsEnabled event is sent
        //! once the entity is participating fully in the physics simulation.
        virtual void EnablePhysics() = 0;

        //! Stop the entity from participating in the physics simulation
        virtual void DisablePhysics() = 0;

        //! Is Physics enabled on this entity?
        virtual bool IsPhysicsEnabled() = 0;

        //! Apply an impulse to the entity.
        //! \param impulse - world-space change in angular velocity.
        virtual void AddImpulse(const AZ::Vector3& /*impulse*/) { }
        //! Apply an impulse to the entity.
        //! \param impulse - world-space change in angular velocity.
        //! \param worldSpacePoint - world space point at which impulse is applied. Specify a zero Vector3 to apply at center of mass.
        virtual void AddImpulseAtPoint(const AZ::Vector3& /*impulse*/, const AZ::Vector3& /*worldSpacePoint*/) { }
        //! Apply an impulse to the entity.
        //! \param impulse - world-space change in angular velocity.
        virtual void AddAngularImpulse(const AZ::Vector3& /*impulse*/) { }

        //! Get the velocity of the entity
        virtual AZ::Vector3 GetVelocity() { return AZ::Vector3::CreateZero(); }
        //! Set the velocity of the entity
        //! \param velocity - world-space velocity.
        virtual void SetVelocity(const AZ::Vector3& /*velocity*/) { }
 
        //! Get the angular velocity of the entity
        virtual AZ::Vector3 GetAngularVelocity() { return AZ::Vector3::CreateZero(); }
        //! Set the angular velocity of the entity
        //! \param angularVelocity - world-space angular velocity to directly set on the entity.
        virtual void SetAngularVelocity(const AZ::Vector3& /*angularVelocity*/) { }

        //! Get the total mass of the entity, in kg
        virtual float GetMass() { return 0.0f; }
        //! Set the total mass of the entity, in kg
        //! \param mass
        virtual void SetMass(float /*mass*/) { };

        //! Get the simulation damping coefficient of the entity
        virtual float GetLinearDamping() { return 0.0f; }

        //! Set entity's damping coefficient for simulation
        //! \param damping - entity's damping coefficient for simulation
        virtual void SetLinearDamping(float /*damping*/) { }

        //! Get the simulation damping coefficient of the entity
        virtual float GetAngularDamping() { return 0.0f; }

        //! Set entity's damping coefficient for simulation
        //! \param damping - entity's damping coefficient for simulation
        virtual void SetAngularDamping(float /*damping*/) { }

        //! Get the minimum kinetic energy below which the entity should fall asleep
        virtual float GetSleepThreshold() { return 0.0f; }

        //! Set entity's minimum kinetic energy below which the entity should fall asleep
        //! \param threshold- minimum kinetic energy
        virtual void SetSleepThreshold(float /*threshold*/) { }

        //! Get the AABB of the entity in world space
        virtual AZ::Aabb GetAabb() { return AZ::Aabb::CreateNull(); }

        //! Get the RigidBody object from the component
        virtual Physics::RigidBody* GetRigidBody() { return nullptr; }

        // Deprecated functions

        //! Deprecated.
        //! @deprecated Please use GetLinearDamping and GetAngularDamping instead.
        virtual float GetDamping() { return GetLinearDamping(); }
        
        //! Deprecated.
        //! @deprecated Please use SetLinearDamping and SetAngularDamping instead.
        virtual void SetDamping(float damping) { SetLinearDamping(damping); }
        
        //! Deprecated.
        //! @deprecated Please use GetSleepThreshold instead.
        virtual float GetMinEnergy() { return GetSleepThreshold(); }

        //! Deprecated.
        //! @deprecated Please use SetSleepThreshold instead.
        virtual void SetMinEnergy(float minEnergy) { SetSleepThreshold(minEnergy); }

        //! Deprecated.
        //! @deprecated Please use AddAngularImpulse or AddImpulse instead.
        virtual void AddAngularImpulseAtPoint(const AZ::Vector3& /*impulse*/, const AZ::Vector3& /*worldSpacePivot*/) {}

        //! Deprecated.
        //! @deprecated Please use CryPhysicsComponentRequestBus::GetWaterDamping instead.
        virtual float GetWaterDamping() { return 0.0f; }

        //! Deprecated.
        //! @deprecated Please use CryPhysicsComponentRequestBus::SetWaterDamping instead.
        virtual void SetWaterDamping(float /*waterDamping*/) { }

        //! Deprecated.
        //! @deprecated Please use CryPhysicsComponentRequestBus::GetWaterDensity instead.
        virtual float GetWaterDensity() { return 0.0f; }

        //! Deprecated.
        //! @deprecated Please use CryPhysicsComponentRequestBus::SetWaterDensity instead.
        virtual void SetWaterDensity(float /*waterDensity*/) {}

        //! Deprecated.
        //! @deprecated Please use CryPhysicsComponentRequestBus::GetWaterResistance instead.
        virtual float GetWaterResistance() { return 0.0f; }

        //! Deprecated.
        //! @deprecated Please use CryPhysicsComponentRequestBus::SetWaterResistance instead.
        virtual void SetWaterResistance(float /*waterResistance*/) {}

        //! Deprecated.
        //! @deprecated Please use CryPhysicsComponentRequestBus::GetAcceleration instead.
        virtual AZ::Vector3 GetAcceleration() { return AZ::Vector3::CreateZero(); }

        //! Deprecated.
        //! @deprecated Please use CryPhysicsComponentRequestBus::GetAngularAcceleration instead.
        virtual AZ::Vector3 GetAngularAcceleration() { return AZ::Vector3::CreateZero(); }

        //! Deprecated.
        //! @deprecated Please use CryPhysicsComponentRequestBus::GetDensity instead.
        virtual float GetDensity() { return 0.0f; }

        //! Deprecated.
        //! @deprecated Please use CryPhysicsComponentRequestBus::SetDensity instead.
        virtual void SetDensity(float /*density*/) {}
    };
    using PhysicsComponentRequestBus = AZ::EBus<PhysicsComponentRequests>;

    /*!
    * Events emitted by a physics component and by the physics system.
    * 
    * Note: OnPhysicsEnabled will fire immediately upon connecting to the bus if physics is enabled
    */
    class PhysicsComponentNotifications
        : public AZ::ComponentBus
    {
    public:
        template<class Bus>
        struct ConnectionPolicy
            : public AZ::EBusConnectionPolicy<Bus>
        {
            static void Connect(typename Bus::BusPtr& busPtr, typename Bus::Context& context, typename Bus::HandlerNode& handler, const typename Bus::BusIdType& id = 0)
            {
                AZ::EBusConnectionPolicy<Bus>::Connect(busPtr, context, handler, id);

                bool physicsEnabled = false;
                EBUS_EVENT_ID_RESULT(physicsEnabled, id, PhysicsComponentRequestBus, IsPhysicsEnabled);
                if (physicsEnabled)
                {
                    typename Bus::template CallstackEntryIterator<typename Bus::InterfaceType**> callstack(nullptr, &id); // Workaround for GetCurrentBusId in callee
                    handler->OnPhysicsEnabled();
                }
            }
        };

        virtual ~PhysicsComponentNotifications() = default;

        /// The entity has begun participating in the physics simulation.
        /// If the entity is enabled when a handler connects to the bus,
        /// then OnPhysicsEnabled() is immediately dispatched.
        /// Note that this event is never sent for improperly configured entities
        /// that cannot participate in the physics simulation.
        /// For example, an entity with no collision is improperly configured.
        virtual void OnPhysicsEnabled() {};

        //! The entity is stopping participation in the physics simulation.
        virtual void OnPhysicsDisabled() {};

        //! Information about the collision that occurred
        //! NOTE: m_velocity[A|B] and m_mass[A|B] will be deprecated in the near future, to be replaced with collections m_velocities and m_masses
        struct Collision
        {
            AZ_TYPE_INFO(Collision, "{33756BD4-24D4-4DAE-A849-537114D52F7D}");
            AZ_CLASS_ALLOCATOR(Collision, AZ::SystemAllocator, 0);

            AZ::EntityId m_entity;      //!< ID of other entity involved in event
            AZ::Vector3 m_position;     //!< Contact point in world coordinates
            AZ::Vector3 m_normal;       //!< Normal to the collision
            float m_impulse;            //!< Impulse applied by the collision resolver
            AZStd::array<AZ::Vector3, 2> m_velocities;    //!< Velocities of the entities involved in the collision
            AZStd::array<float, 2> m_masses;              //!< Masses of the entities involved in the collision
            AZStd::array<int, 2> m_surfaces;    //!< Surfaces of the entities involved in the collision
        };

        //! The entity has collided with another entity (\ref otherEntity), with the collision occurring at \ref location.
        virtual void OnCollision(const Collision& collision) { }
    };
    using PhysicsComponentNotificationBus = AZ::EBus<PhysicsComponentNotifications>;

    /**
     * The type ID of RigidPhysicsComponent
     */
    static const AZ::Uuid RigidPhysicsComponentTypeId = "{BF2ED241-6364-4D78-8008-498EF2A2659C}";

    /**
     * The type ID of EditorRigidPhysicsComponent
     */
    static const AZ::Uuid EditorRigidPhysicsComponentTypeId = "{BD17E257-BADB-45D7-A8BA-16D6B0BE0881}";

    /**
    * The type ID of StaticPhysicsComponent
    */
    static const AZ::Uuid StaticPhysicsComponentTypeId = "{95D89791-6397-41BC-AAC5-95282C8AD9D4}";

    /**
    * The type ID of EditorStaticPhysicsComponent
    */
    static const AZ::Uuid EditorStaticPhysicsComponentTypeId = "{C8D8C366-F7B7-42F6-8B86-E58FFF4AF984}";

    /**
     * Configuration data for RigidPhysicsComponent.
     */
    class RigidPhysicsConfig
        : public AZ::ComponentConfig
    {
    public:
        AZ_CLASS_ALLOCATOR(RigidPhysicsConfig, AZ::SystemAllocator, 0);
        AZ_RTTI(RigidPhysicsConfig, "{4D4211C2-4539-444F-A8AC-B0C8417AA579}", AZ::ComponentConfig);

        static void Reflect(AZ::ReflectContext* context);

        //! Whether total mass is specified, or calculated at spawn time based on density and volume
        enum class MassOrDensity : uint32_t
        {
            Mass,
            Density,
        };

        bool UseMass() const
        {
            return m_specifyMassOrDensity == MassOrDensity::Mass;
        }

        bool UseDensity() const
        {
            return m_specifyMassOrDensity == MassOrDensity::Density;
        }

        //! Whether the entity is initially enabled in the physics simulation.
        //! Entities can be enabled later via PhysicsComponentRequests::Enable(true).
        bool m_enabledInitially = true;

        //! Whether total mass is specified, or calculated at spawn time based on density and volume
        MassOrDensity m_specifyMassOrDensity = MassOrDensity::Density;

        //! Total mass of entity, in kg
        float m_mass = 10.f; // 10kg would be a cube of water with 10cm sides.

        //! Density of the entity, in kg per cubic meter.
        //! The total mass of entity will be calculated at spawn,
        //! based on density and the volume of its geometry.
        //! Mass = Density * Volume
        float m_density = 500.f; // 1000 kg/m^3 is density of water. Oak is 600 kg/m^3.

        //! Whether the entity is initially considered at-rest in the physics simulation.
        //! True: Entity remains still until agitated.
        //! False: Entity will be fall if nothing is below it.
        bool m_atRestInitially = false;

        //! Entity will physically react to collisions, rather than only report them.
        bool m_enableCollisionResponse = true;

        //! Indicates whether this component can interact with proximity triggers
        bool m_interactsWithTriggers = true;

        //! Damping value applied while in water.
        float m_buoyancyDamping = 0.f;

        //! Water density strength.
        float m_buoyancyDensity = 1.f;

        //! Water resistance strength.
        float m_buoyancyResistance = 1.f;

        //! Movement damping.
        float m_simulationDamping = 0.f;

        //! Minimum energy under which object will sleep.
        float m_simulationMinEnergy = 0.002f;

        //! Whether or not to record collisions on this entity.
        bool m_recordCollisions = true;

        //! Maximum number of collisions to be recorded per frame.
        int m_maxRecordedCollisions = 1;
    };

    /*!
     * Configuration data for StaticPhysicsComponent.
     */
    class StaticPhysicsConfig
        : public AZ::ComponentConfig
    {
    public:
        AZ_CLASS_ALLOCATOR(StaticPhysicsConfig, AZ::SystemAllocator, 0);
        AZ_RTTI(StaticPhysicsConfig, "{2129576B-A548-4F3E-A2A1-87851BF48838}", AZ::ComponentConfig);
        static void Reflect(AZ::ReflectContext* context);

        //! Whether the entity is initially enabled in the physics simulation.
        //! Entities can be enabled later via PhysicsComponentRequests::Enable(true).
        bool m_enabledInitially = true;
    };

} // namespace AzFramework

namespace AZ
{
    AZ_TYPE_INFO_SPECIALIZE(AzFramework::RigidPhysicsConfig::MassOrDensity, "{0F5DBFB3-FD9A-4E83-B9B3-4713AB2241B4}");
}