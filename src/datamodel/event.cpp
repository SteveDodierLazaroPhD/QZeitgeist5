/*
 * This file is part of QtZeitgeist.
 *
 * Copyright (C) 2010 Collabora Ltd. <http://www.collabora.co.uk/>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */


#include "QtZeitgeist/DataModel/Event"

#include <QMetaType>
#include <QDBusMetaType>

namespace QtZeitgeist
{

namespace DataModel
{

class EventPrivate
{
public :
    EventPrivate()
    {
        id = 0;
        timestamp.setTimeSpec(Qt::UTC);
        timestamp = QDateTime::currentDateTime();
    }

    quint32 id;
    QDateTime timestamp;
    QString interpretation;
    QString manifestation;
    QString actor;
    QList<QStringList> subjects;
    QByteArray payload;
};

Event::Event(QObject *parent)
    : d(new EventPrivate())
{
    Q_ASSERT(d);
}

Event::Event(const Event & source, QObject *parent)
    : d(new EventPrivate())
{
    Q_ASSERT(d);

    // Copy the source attribute's value.
    d->id = source.d->id;
    d->timestamp = source.d->timestamp;
    d->interpretation = source.d->interpretation;
    d->manifestation = source.d->manifestation;
    d->actor = source.d->actor;
    d->subjects = source.d->subjects;
    d->payload = source.d->payload;
}

Event::~Event()
{
    delete d;
}

quint32 Event::id() const
{
    return d->id;
}

QDateTime Event::timestamp() const
{
    return d->timestamp;
}

void Event::setTimestamp(const QDateTime &timestamp)
{
    d->timestamp = timestamp;
}

QString Event::interpretation() const
{
    return d->interpretation;
}

void Event::setInterpretation(const QString &interpretation)
{
    d->interpretation = interpretation;
}

QString Event::manifestation() const
{
    return d->manifestation;
}

void Event::setManifestation(const QString &manifestation)
{
    d->manifestation = manifestation;
}

QString Event::actor() const
{
    return d->actor;
}

void Event::setActor(const QString &actor)
{
    d->actor = actor;
}

QList<QStringList> Event::subjects() const
{
    return d->subjects;
}

void Event::setSubjects(const QList<QStringList> &subjects)
{
    d->subjects = subjects;
}

QByteArray Event::payload() const
{
    return d->payload;
}

void Event::setPayload(const QByteArray &payload)
{
    d->payload = payload;
}

void Event::addSubject(const QStringList &subject)
{
    d->subjects.append(subject);
}

Event &Event::operator = (const Event & source)
{
    // Copy the source attribute's value.
    if (this != &source) {
        d->id = source.d->id;
        d->timestamp = source.d->timestamp;
        d->interpretation = source.d->interpretation;
        d->manifestation = source.d->manifestation;
        d->actor = source.d->actor;
        d->subjects = source.d->subjects;
        d->payload = source.d->payload;
    }

    return *this;
}

QDBusArgument & operator << (QDBusArgument &argument, const Event &event)
{
    QStringList eventData;
    event.d->timestamp = QDateTime::currentDateTime();

    eventData
        << QString(event.d->id)
        << QString::number(event.d->timestamp.toTime_t() * 1000)
        << event.d->interpretation
        << event.d->manifestation
        << event.d->actor;

    argument.beginStructure();

    argument
        << eventData
        << event.d->subjects
        << event.d->payload;

    argument.endStructure();

    return argument;
}

const QDBusArgument & operator >> (const QDBusArgument &argument, Event &event)
{
    QStringList eventData;

    argument.beginStructure();

    argument
        >> eventData
        >> event.d->subjects
        >> event.d->payload;

    argument.endStructure();

    if (!eventData.isEmpty()) {
        event.d->id = eventData[0].toUInt();

        event.d->timestamp.setTime_t(0);
        event.d->timestamp.addMSecs(eventData[1].toLongLong());

        event.d->interpretation = eventData[2];
        event.d->manifestation = eventData[3];
        event.d->actor = eventData[4];
    }

    return argument;
}

};

};