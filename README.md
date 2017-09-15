# QMikHSManager
A remote hotspot manager for Mikrotik ROS

This will become a small software to allow any user to manage a mikrotik hotspot.
With this program and a very basic hotspot server running, you can do many things such as:

(This is a very preliminar features I'm willing to implement)
1. Create/delete/modify new users with a controlled pattern for user and password (TODO)
2. Create many users at a time (TODO)
3. Monitorize user activity and traffic usage per user (In progress)
4. Add the absolute time limit feature (not present in basic ROS hotspot) (TODO)
5. Auto delete and disconnect timed out users. (TODO)
6. Printing tickets (TODO)

For the delete/disconnect timed out users feature, this program must remain connected to the ROS.
This is my decission, as I prefer this way instead of activate a schedule and make the ROS looks
for timed out users. I think ROS CPU will agree with me. Don't you?

It uses the QMikAPI code files that have demonstrated (to me, at least) being bug free and very usable.

Any suggestions are quite welcome.
