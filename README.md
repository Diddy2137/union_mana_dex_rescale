# change the way how ranged weapons and magic work

based on https://github.com/Gratt-5r2/UnionProject

# union_mana_dex_rescale</br>
if you want to mess around with the damage formula etc</br>
UnionProject/Plugin/plugin.cpp </br>
i was to lazy to move things around so everything stayed in the plugin.cpp file </br>
this plugin should work with almost any mod if it doesn't the mod already changes how the damage scaling work</br>
the plugin breaks if something else is messing with 
$${\color{red}oCNpc::OnDamage (oSDamageDescriptor)}$$
