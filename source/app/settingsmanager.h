/*
  * Copyright (C) 2015 Cameron White
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef APP_SETTINGSMANAGER_H
#define APP_SETTINGSMANAGER_H

#include <boost/filesystem/path.hpp>
#include <boost/signals2/signal.hpp>
#include <mutex>
#include <util/settingstree.h>

class SettingsManager
{
public:
    typedef boost::signals2::signal<void()> SettingsChangedSignal;

    /// Handle to ensure the settings aren't modified while being accessed by
    /// another thread (e.g. the MIDI thread).
    template <typename T>
    class Handle
    {
    public:
        Handle(const Handle &) = delete;
        Handle& operator=(const Handle &) = delete;

        // TODO - change to a defaulted move constructor when VS2013 is no
        // longer supported.
        Handle(Handle &&other)
            : mySettings(other.mySettings), myLock(std::move(other.myLock))
        {

        }

        T *operator->() const { return &mySettings; }
        T &operator*() const { return mySettings; }

    protected:
        Handle(T &settings, std::mutex &mutex)
            : mySettings(settings), myLock(mutex)
        {
        }

        friend class SettingsManager;

        T &mySettings;
        std::unique_lock<std::mutex> myLock;
    };

    using ReadHandle = Handle<const SettingsTree>;

    class WriteHandle : public Handle<SettingsTree>
    {
    public:
        WriteHandle(SettingsManager &manager)
            : Handle(manager.mySettings, manager.myMutex),
              mySignal(manager.mySettingsChangedSignal)
        {
        }

        ~WriteHandle()
        {
            // Unlock before signalling to avoid deadlocks if callbacks read the
            // settings.
            myLock.unlock();
            mySignal();
        }

        // TODO - change to a defaulted move constructor when VS2013 is no
        // longer supported.
        WriteHandle(WriteHandle &&other)
            : Handle<SettingsTree>(std::move(other)), mySignal(other.mySignal)
        {
        }

    private:
        SettingsChangedSignal &mySignal;
    };

    SettingsManager() = default;
    SettingsManager(const SettingsManager &) = delete;
    SettingsManager &operator=(const SettingsManager &) = delete;

    /// Obtain read access to the settings.
    ReadHandle getReadHandle() const
    {
        return ReadHandle(mySettings, myMutex);
    }

    /// Obtain write access to the settings.
    WriteHandle getWriteHandle()
    {
        return WriteHandle(*this);
    }

    /// Register a callback when a setting is changed.
    boost::signals2::connection subscribeToChanges(
        const SettingsChangedSignal::slot_type &slot)
    {
        return mySettingsChangedSignal.connect(slot);
    }

    /// Load the settings from the specified directory.
    void load(const boost::filesystem::path &dir);

    /// Save the settings to the specified directory.
    void save(const boost::filesystem::path &dir) const;

private:
    template <typename T>
    friend class Handle;

    SettingsTree mySettings;
    mutable std::mutex myMutex;

    SettingsChangedSignal mySettingsChangedSignal;
};

#endif
