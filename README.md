# ThirdPersonMP

Tutorial and learn Third Person in Multiplayer
Source Docs: https://docs.unrealengine.com/en-US/Gameplay/Networking/QuickStart/index.html

# Pattern on replicates system

- An external Actor or function calls CauseDamage on our Character, which in turn calls its **TakeDamage** function.
- **TakeDamage** calls **SetCurrentHealth** to change the player's Current Health value on the server.
- **SetCurrentHealth** calls **OnHealthUpdate** on the server, causing any functionality that happens in response to changes in the player's health to execute.
- **CurrentHealth** replicates to all connected clients' copies of the Character.
- When each client receives a new **CurrentHealth** value from the server, they call **OnRep_CurrentHealth**.
- **OnRep_CurrentHealth** calls **OnHealthUpdate**, ensuring that each client responds the same way to the new **CurrentHealth** value.

## Tips and Tricks

### Register another function to impact event.

```C++
//Registering the Projectile Impact function on a Hit event.
SphereComponent->OnComponentHit.AddDynamic(this, &AThirdPersonMPProjectile::OnProjectileImpact);
```

Note: Function need signature
```C++
OnProjectileImpact(UPrimitiveComponent* ,AActor* ,UPrimitiveComponent* , FVector , const FHitResult&)
```

### Autoreplicates spawn and destroy function 
The Destroyed function is called any time an Actor is destroyed. Particle emitters themselves do not normally replicate, but since Actor destruction does replicate, we know that if we destroy this projectile on the server then this function will be called on each connected client when they destroy their own copies of it. As a result, all players will see the explosion effect when the projectile is destroyed.
```C++
void AThirdPersonMPProjectile::Destroyed()
{
    FVector spawnLocation = GetActorLocation();
    UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionEffect, spawnLocation, FRotator::ZeroRotator, true, EPSCPoolMethod::AutoRelease);
}
```