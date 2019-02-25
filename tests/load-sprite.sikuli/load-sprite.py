import shutil

try:
    import org.sikuli.script.SikulixForJython
    from sikuli import *
except ImportError:
    print("No run by sikuli jython")

fn= inspect.getsourcefile(lambda:0)
path= os.path.dirname(os.path.abspath(fn))
bsfn= os.path.splitext(os.path.basename(fn))[0]
addImagePath(path)

def main():
    spriteFName= "gg006.spr" # crab
    wait("ctp2start-scr.png", 100) # still works with sprite button shown
    click("ctp2sprite-test-btn.png")
    wait(10) # progress bar does not appear at once
    if waitVanish(Pattern("ctp2progress-bar.png").similar(0.95), 100): # wait until progressbar vanishes
        wait(10) # some more time
        click(Pattern("ctp2sprite-sprite-name-field.png").similar(0.95).targetOffset(0, -10)) # text field needs click to get focus
        Settings.TypeDelay = 0.1; # ctp2-SDL needs some more time
        type(spriteFName)
        click("ctp2sprite-load-btn.png")
        wait(10)
        if exists("ctp2sprite-crab-start-img.png", 100):
            file = capture(SCREEN.getBounds())
            if file:
                f= Finder(file) # http://doc.sikuli.org/finder.html
                f.find("ctp2sprite-crab-start-img.png")
                if not f.hasNext():
                    print("Pattern not found in screen shot: " + file)
                    exit(30)
                shutil.move(file, bsfn + '.png')
                exit(0)
        else:
            file = capture(SCREEN.getBounds())
            shutil.move(file, bsfn + '_.png')
            exit(10)
    else:
        exit(20)
    exit(99)

if __name__ == "__main__":
    main()
