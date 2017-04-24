# Missing Lands

![screenie](http://i.imgur.com/3Ia5k5m.gifv)

This is the code behind a little game called Missing Lands. It was built in 48 hours for Ludum Dare.

It is based off of [nes-c-boilerplate](https://github.com/cppchriscpp/nes-c-boilerplate). If you are attempting to
work with the source code, I strongly recommend looking at the readme for that. 

Check it out on [the Ludum Dare Website](https://ldjam.com/events/ludum-dare/38/missing-lands)

Or [play it](http://cpprograms.net/classic-gaming/missing-lands/) on my site.

Better yet, [download the rom](http://cpprograms.net/devnull/world.nes) and play it yourself. The web-based emulator
has some issues with lag and input delay, so I recommend this option more, especially if you like it at all.

## Building

The instructions for building this are nearly identical to those of 
[nes-c-boilerplate](https://github.com/cppchriscpp/nes-c-boilerplate).
You should be able to follow those verbatim to get up and running.

*Note*: You will need to have nodejs installed on your system to run the level builder. This requirement differs
        from nes-c-boilerplate.

## Continuous Integration

This repository is linked in with CircleCI. As such, any changes to master are automatically compiled into a rom.

[Download Latest ROM](https://circleci-tkn.rhcloud.com/api/v1/project/cppchriscpp/missing-lands/tree/master/latest/artifacts/hworld.nes)

## License

All code/resources are available under the MIT license. Happy hacking!